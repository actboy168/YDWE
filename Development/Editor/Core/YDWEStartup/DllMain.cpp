#include <list>
#include <utility>
#include <locale>
#include <windows.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/exception/all.hpp>
#include <CImg.h>
#include <ydwe/file/memory_mapped_file.h>
#include <ydwe/exception/system_exception.h>
#include <ydwe/exception/windows_exception.h>
#include <ydwe/i18n/libintl.h>
#include <ydwe/path/service.h>
#include <ydwe/util/unicode.h>
#include <ydwe/win/file_version.h>
#include <ydwe/win/process.h>
#include <YDWEConfig/Warcraft3Directory.h>
#include <YDWEConfig/Warcraft3Directory.cpp>

namespace fs = boost::filesystem;

#define _(str)  ydwe::i18n::gettext(str).to_string().c_str()
#define __(str) ydwe::util::u2w_ref(ydwe::i18n::gettext(str)).c_str()

static bool FileContentEqual(const boost::filesystem::path &fileFirst, const boost::filesystem::path &fileSecond, std::error_code *pErrorCode = nullptr)
{
	try
	{
		if (pErrorCode)
		{
			pErrorCode->assign(boost::system::errc::success, std::generic_category());
		}

		ydwe::file::memory_mapped_file mapperFirst(fileFirst.c_str());
		ydwe::file::memory_mapped_file mapperSecond(fileSecond.c_str());

		size_t size;
		return (size = mapperFirst.size()) == mapperSecond.size() && memcmp(mapperFirst.memory(), mapperSecond.memory(), size) == 0;
	}
	catch (ydwe::system_exception const& e)
	{
		if (pErrorCode)
		{
			*pErrorCode = e.error_code();
		}

		return false;
	}
}

static void ShowSplash(fs::path const& ydwe_path)
{
	ydwe::win::simple_file_version fv((ydwe_path / "YDWE.exe").c_str());

	// Get image path
	fs::path splashPath = ydwe_path / L"bin" / L"splash.bmp";
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
	{
		return;
	}

	if (fs::exists(destination))
	{
		if (fs::file_size(source) == fs::file_size(destination))
		{
			if (FileContentEqual(source, destination))
				return;
		}
	}

	fs::copy_file(source, destination, fs::copy_option::overwrite_if_exists);
}

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

static void DoTask()
{
	fs::path gExecutableDirectory = ydwe::path::get(ydwe::path::DIR_EXE).remove_filename();

	ydwe::i18n::textdomain("YDWEStartup");
	ydwe::i18n::bindtextdomain("YDWEStartup", gExecutableDirectory / L"share" / L"locale");

	if (gExecutableDirectory != fs::path(gExecutableDirectory.string()))
	{
		BOOST_THROW_EXCEPTION(std::domain_error(_("Error YDWE directory.")));
	}

	fs::path gWarcraftDirectory;
	if (!warcraft3_directory::get(__("Please choose your Warcraft 3 installation directory"), gWarcraftDirectory))
	{
		return ;
	}

	MoveDetouredSystemDll(gWarcraftDirectory);

	// Set current directory
	SetCurrentDirectoryW(gExecutableDirectory.c_str());

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


	ydwe::win::process worldedit_process;
	bool result = worldedit_process.create(worldeditPreferredPath, std::wstring(::GetCommandLineW()));

	if (!result)
	{
		throw ydwe::windows_exception(_("Failed to launch world editor."));
	}

	ShowSplash(gExecutableDirectory);
}

INT WINAPI YDWEStartup(HINSTANCE current, HINSTANCE previous, LPSTR pCommandLine, INT showType)
{
	INT exitCode = -1;

	try
	{
		DoTask();
		exitCode = 0;
	}
	catch (std::exception const& e)
	{
		MessageBoxW(NULL, ydwe::util::u2w_ref(e.what()).c_str(), __("Error"), MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBoxW(NULL, __("Unknown error"), __("Error"), MB_OK | MB_ICONERROR);
	}

	return exitCode;
}
