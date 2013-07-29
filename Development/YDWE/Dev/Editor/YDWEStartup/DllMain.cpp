#include <list>
#include <utility>
#include <locale>
#include <WinSock2.h>
#include <windows.h>
#include <stlsoft/stlsoft.h>
#include <winstl/winstl.h>
#include <winstl/registry/registry.hpp>
#include <winstl/registry/reg_key.hpp>
#include <winstl/registry/reg_value.hpp> 
#include <winstl/error/error_desc.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/exception/all.hpp>
#include <CImg.h>
#include <shlwapi.h>
#include <ydwe/i18n/libintl.h>
#include <ydwe/path/service.h>
#include <ydwe/util/unicode.h>
#include <ydwe/win/file_version.h>
#include "FileCheck.h"


using namespace std;
namespace fs = boost::filesystem;

#define _(str) ydwe::util::u2a(ydwe::i18n::gettext(str)).c_str()
#define __(str) ydwe::util::u2w(ydwe::i18n::gettext(str)).c_str()

/// Self directory
static fs::path gExecutableDirectory;
/// Warcraft directory
static fs::path gWarcraftDirectory;

/** \brief Searches executable path
* 
* Search the main executable path
*
* \return The executable path
* \exception std::domain_error If cannot found
*/
static fs::path FindExecutablePath()
{
	wchar_t buffer[MAX_PATH];

	if (GetModuleFileNameW(NULL, buffer, sizeof(buffer) / sizeof(buffer[0])) == 0)
	{
		BOOST_THROW_EXCEPTION(std::domain_error(_("Cannot find YDWE directory.")));
	}

	return std::move(fs::path(buffer));
}

static bool RunOpenFileDialog(LPCWSTR title, LPCWSTR filter, HWND owner, std::wstring* path) 
{
	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = owner;
	wchar_t filename[MAX_PATH];
	filename[0] = 0;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = _countof(filename);
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	if (title) ofn.lpstrTitle = title;
	if (filter) ofn.lpstrFilter = filter;

	if (!GetOpenFileNameW(&ofn))
	{
		return false;
	}

	*path = std::wstring(filename);
	return true;
}

static bool ChooseWarcraftDirectory()
{
	typedef winstl::basic_reg_key<wchar_t> TRegKey;

	std::wstring retval;
	if (!RunOpenFileDialog(__("Please choose your Warcraft 3 installation directory"), L"war3.exe\0war3.exe\0", NULL, &retval))
	{
		return false;
	}

	bool success;
	try
	{
		TRegKey warReg = TRegKey::create_key(HKEY_CURRENT_USER, L"Software\\Blizzard Entertainment\\Warcraft III");
		warReg.set_value(L"InstallPath", fs::path(retval).parent_path().c_str());
		success = true;
	}
	catch (...)
	{
		success = false;
	}

	return success;
}

static bool ReadWarcraftDirectory(fs::path& result)
{
	typedef winstl::basic_reg_key<wchar_t> TRegKey;
	typedef winstl::basic_reg_value<wchar_t> TRegValue;

	bool success;

	try
	{
		TRegKey warReg = TRegKey(HKEY_CURRENT_USER, L"Software\\Blizzard Entertainment\\Warcraft III");
		TRegValue regValue = warReg.get_value(L"InstallPath");
		result = regValue.value_sz().c_str();

		success = true;
	}
	catch (...)
	{
		success = false;
	}

	if (!success || !fs::exists(result))
	{
		success = false;
	}

	return success;
}

// War3 1.26会自动将自身路径写入注册表，但写入的位置是HKEY_LOCAL_MACHINE
static bool ReadWarcraftDirectoryAllUser(fs::path& result)
{
	typedef winstl::basic_reg_key<wchar_t> TRegKey;
	typedef winstl::basic_reg_value<wchar_t> TRegValue;

	bool success;

	try
	{
		TRegKey warReg = TRegKey(HKEY_LOCAL_MACHINE, L"Software\\Blizzard Entertainment\\Warcraft III", KEY_READ);
		TRegValue regValue = warReg.get_value(L"InstallPath");
		result = regValue.value_sz().c_str();

		success = true;
	}
	catch (...)
	{
		success = false;
	}

	if (!success || !fs::exists(result))
	{
		success = false;
	}

	return success;
}

static bool ValidateWarcraftDirectory(const fs::path &warcraftDirectory)
{
	using namespace boost::assign; // bring '+=' into scope

	static const std::list<std::wstring> fileList
		= list_of(L"war3.exe")(L"game.dll")(L"war3.mpq")(L"war3patch.mpq")(L"storm.dll");

	BOOST_FOREACH(const std::wstring &fileName, fileList)
	{
		fs::path fileToCheck = warcraftDirectory / fileName;
		if (!fs::exists(fileToCheck))
		{
			return false;
		}
	}

	return true;
}

/** \brief Searches warcraft directory
* 
* Search the warcraft directory
*
* \return The warcraft directory
* \exception std::domain_error If cannot found
*/
static fs::path FindWarcraftDirectory()
{
	fs::path result;
	if (ReadWarcraftDirectory(result))
	{
		bool success = ValidateWarcraftDirectory(result);
		if (success)
			return std::move(result);
	}

	if (ReadWarcraftDirectoryAllUser(result))
	{
		bool success = ValidateWarcraftDirectory(result);
		if (success)
			return std::move(result);
	}

	if (ChooseWarcraftDirectory())
	{
		if (ReadWarcraftDirectory(result))
		{
			bool success = ValidateWarcraftDirectory(result);

			if (!success)
				BOOST_THROW_EXCEPTION(std::domain_error(_("Cannot find some important files(war3.exe, game.dll, etc.) in your Warcraft 3 directory. Please make sure you have copied the fixer to you Warcraft 3 directory and run it."))
					);
			return std::move(result);
		}
	}

	BOOST_THROW_EXCEPTION(std::domain_error(_("Cannot find Warcraft 3 installation path. Please check if your registry contains enough information about your Warcraft 3. Use registry fixer if needed.")));

	return std::move(result);
}


/** \brief Show splash screen
*
* Show splash screen
*/
static void ShowSplash(fs::path const& executablePath)
{
	ydwe::win::simple_file_version fv(executablePath.c_str());

	// Get image path
	fs::path splashPath = gExecutableDirectory / L"bin" / L"splash.bmp";
	if (fs::exists(splashPath))
	{
		FILE* f = _wfopen(splashPath.c_str(), L"rb");
		if (f)
		{
			try {
				cimg_library::CImg<boost::uint8_t> splashImage = cimg_library::CImg<boost::uint8_t>::get_load_bmp(f);
				boost::uint8_t color[] = { 255, 255, 255 };
				splashImage.draw_text(10, 10, (boost::format("YDWE %1$d.%2$d.%3$d.%4$d") % fv.major % fv.minor % fv.revision % fv.build).str().c_str(), color, 0, 1, 20);
				cimg_library::CImgDisplay display(splashImage, "YDWE", 3, false, true);
				display.move(
					(cimg_library::CImgDisplay::screen_width() - display.width()) / 2,
					(cimg_library::CImgDisplay::screen_height() - display.height()) / 2
					).hide_mouse();
				display.show();
				Sleep(1000);
				display.show_mouse();
			} catch (...) { }
			
			fclose(f);
		}
	}
}

static void CheckedCopyFile(const fs::path &source, const fs::path &destination)
{
	if (!fs::exists(source))
		goto LReturn;

	if (!fs::exists(destination))
		goto LCopy;

	auto correctSize = fs::file_size(source), currentSize = fs::file_size(destination);

	// If the same, do not copy
	if (currentSize == correctSize && NYDWE::FileContentEqual(source, destination))
		goto LReturn;

LCopy:
	fs::copy_file(source, destination, fs::copy_option::overwrite_if_exists);

LReturn:
	return;
}

#ifdef NDEBUG
/// Copy CRT to Warcraft directoryto prevent CRT dll confliction
static void CopyCRTToWarcraft()
{
	fs::path source = gExecutableDirectory / L"bin" / L"msvcr100.dll";
	fs::path desination = gWarcraftDirectory / L"msvcr100.dll";
	CheckedCopyFile(source, desination);

	source = gExecutableDirectory / L"bin" / L"msvcp100.dll";
	desination = gWarcraftDirectory / L"msvcp100.dll";
	CheckedCopyFile(source, desination);
}
#endif


const wchar_t* szSystemDllList[] = {
	L"version.dll",
	L"winmm.dll",
};

static void MoveDetouredSystemDll(const fs::path &war3Directory)
{
	fs::path backupPath = war3Directory / L"ydwe_backups_system_dll";
	try
	{
		if (!fs::exists(backupPath))
		{
			fs::create_directory(backupPath);
		}
	}
	catch (...)
	{
		return;
	}

	BOOST_FOREACH(const wchar_t *szDllName, szSystemDllList)
	{
		try
		{
			if (fs::exists(war3Directory / szDllName))
			{
				fs::rename(war3Directory / szDllName, backupPath / szDllName);
			}
		}
		catch (...)
		{}
	}
}

/** \brief Do task
*
* Do actual task
*
* \exception Any exception if something goes wrong
*/
static void DoTask()
{
	// Initialize path
	fs::path executablePath = FindExecutablePath();

	if (executablePath != fs::path(executablePath.string()))
	{
		BOOST_THROW_EXCEPTION(std::domain_error(_("Error YDWE directory.")));
	}

	gExecutableDirectory = executablePath.parent_path();
	gWarcraftDirectory = FindWarcraftDirectory();

	MoveDetouredSystemDll(gWarcraftDirectory);

	// Set current directory
	SetCurrentDirectoryW(gExecutableDirectory.c_str());

	// Verify self
	if (!fs::exists(gExecutableDirectory / L"bin" / L"YDWEInject.dll"))
	{
		BOOST_THROW_EXCEPTION(std::domain_error(_("Cannot find YDWEInject.dll in YDWE/bin directory.")));
	}

	if (fs::exists(gWarcraftDirectory / L"YDDllFixer.dll"))
	{
		if (!fs::remove(gWarcraftDirectory / L"YDDllFixer.dll"))
		{
			BOOST_THROW_EXCEPTION(std::domain_error(_("Cannot delete YDDllFixer.dll in war3 directory.")));
		}
	}

	if (fs::exists(gExecutableDirectory / L"YDDllFixer.dll"))
	{
		if (!fs::remove(gExecutableDirectory / L"YDDllFixer.dll"))
		{
			BOOST_THROW_EXCEPTION(std::domain_error(_("Cannot delete YDDllFixer.dll in ydwe directory.")));
		}
	}

	// Prefered path
	fs::path worldeditPreferredPath = gWarcraftDirectory / L"worldeditydwe.exe";
	// Attached path
	fs::path worldeditAttachedPath = gExecutableDirectory / L"bin" / L"worldedit.exe";

	// Check if attached executable exists
	if (fs::exists(worldeditAttachedPath))
	{
		// Copy it
		CheckedCopyFile(worldeditAttachedPath, worldeditPreferredPath);
	}
	else
	{
		BOOST_THROW_EXCEPTION(std::domain_error(_("Cannot find main executable file of world editor in YDWE/bin directory.")));
	}

	// Show splash screen
	ShowSplash(executablePath);

	// Start it!
	PROCESS_INFORMATION processInformation;
	STARTUPINFOW startupInfo;
	GetStartupInfoW(&startupInfo);

	// Command line buffer
	wchar_t buffer[1024 * 128];

	// Prepare command line
	wcscpy_s(buffer, GetCommandLineW());

	BOOL result = CreateProcessW(
		worldeditPreferredPath.c_str(), buffer,
		NULL, NULL,
		FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL,
		&startupInfo, &processInformation
		);

	if (!result)
	{
		BOOST_THROW_EXCEPTION(std::domain_error((boost::format(
			_("Failed to launch world editor. Error: %1%")
			) % GetLastError()).str().c_str()));
	}
	else
	{
		CloseHandle(processInformation.hThread);
		CloseHandle(processInformation.hProcess);
	}
}

static void InitI18N()
{
	fs::path warcraft3_path = ydwe::path::get(ydwe::path::DIR_EXE).remove_filename();
	ydwe::i18n::textdomain("YDWEStartup");
	ydwe::i18n::bindtextdomain("YDWEStartup", warcraft3_path / L"share" / L"locale");
}

/** \brief Main
*
* Main
*/
INT WINAPI YDWEStartup(HINSTANCE current, HINSTANCE previous, LPSTR pCommandLine, INT showType)
{
	INT exitCode = -1;

	try
	{	
		InitI18N();
		DoTask();
		exitCode = 0;
	}
	catch (std::domain_error &e)
	{
		MessageBoxA(NULL, e.what(), _("Error"), MB_OK | MB_ICONERROR);
	}
	catch (std::exception &e)
	{
		MessageBoxA(NULL, e.what(), _("Error"), MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBoxA(NULL, _("Unknown error"), _("Error"), MB_OK | MB_ICONERROR);
	}

	return exitCode;
}
