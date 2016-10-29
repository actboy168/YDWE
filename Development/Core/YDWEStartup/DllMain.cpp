#include <list>
#include <utility>
#include <locale>
#include <windows.h>
#include <base/filesystem.h>
#include <base/file/memory_mapped_file.h>
#include <base/exception/system_exception.h>
#include <base/exception/windows_exception.h>
#include <base/file/stream.h>
#include <base/i18n/libintl.h>
#include <base/path/service.h>
#include <base/util/unicode.h>
#include <base/win/file_version.h>
#include <base/win/process.h>
#include <base/warcraft3/directory.h>
#include <base/util/format.h>	 
#include <slk/reader/IniReader.hpp>
#include <slk/reader/IniReader.cpp>
#include <slk/reader/CommonReader.cpp>
#include "Splash.h"

#define _(str)  base::i18n::gettext(str).to_string().c_str()
#define __(str) base::u2w(base::i18n::gettext(str)).c_str()

static bool FileContentEqual(const fs::path &fileFirst, const fs::path &fileSecond, std::error_code *pErrorCode = nullptr)
{
	try
	{
		if (pErrorCode)
		{
			pErrorCode->assign(boost::system::errc::success, std::generic_category());
		}

		base::file::memory_mapped_file mapperFirst(fileFirst.c_str());
		base::file::memory_mapped_file mapperSecond(fileSecond.c_str());

		size_t size;
		return ((size = mapperFirst.size()) == mapperSecond.size()) 
			&& (memcmp(mapperFirst.memory(), mapperSecond.memory(), size) == 0);
	}
	catch (base::system_exception const& e)
	{
		if (pErrorCode)
		{
			*pErrorCode = e.code();
		}

		return false;
	}
}

bool Blp2Bmp(const wchar_t* blp, const wchar_t* bmp);

static void ShowSplash(fs::path const& ydwe_path)
{
	fs::path bmp = ydwe_path / L"bin" / L"splash.bmp";
	if (!fs::exists(bmp))
	{
		try {
			slk::IniTable table;
			base::buffer buf = base::file::read_stream(ydwe_path / L"share" / L"mpq" / L"units" / L"ui" / L"worldeditdata.txt").read<base::buffer>();
			base::buffer_reader reader(buf);
			slk::IniReader::Read(reader, table);

			do {
				fs::path blp = ydwe_path / L"share" / L"mpq" / L"units" / table["WorldEditArt"]["War3XLogo"];
				if (fs::exists(blp))
				{
					if (Blp2Bmp(blp.wstring().c_str(), bmp.wstring().c_str()))
						break;
				}
				blp = ydwe_path / L"share" / L"mpq" / L"units" / table["WorldEditArt"]["War3Logo"];
				if (fs::exists(blp))
				{
					if (Blp2Bmp(blp.wstring().c_str(), bmp.wstring().c_str()))
						break;
				}
			} while (false);
		}
		catch (...) {
		}
	}

	if (fs::exists(bmp))
	{
		try {
			base::win::simple_file_version fv((ydwe_path / "YDWE.exe").c_str());
			CSplash display;
			display.SetBitmap(bmp.wstring().c_str());
			display.SetTransparentColor(RGB(128, 128, 128));
			display.SetText(base::format(L"YDWE %d.%d.%d.%d", fv.major, fv.minor, fv.revision, fv.build).c_str(), 10, 10, 10, 20);
			display.Show();
			Sleep(1000);
			display.Close();
		} catch (...) {
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

	for (const wchar_t *szDllName : szSystemDllList)
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

//
// see http://blogs.msdn.com/b/shawnfa/archive/2009/06/08/more-implicit-uses-of-cas-policy-loadfromremotesources.aspx
//
static bool CreateDotNetConfig(fs::path const& config_path)
{
	try {
		base::file::write_stream(config_path).write(std::string(
			"<?xml version=\"1.0\"?>"						    "\n"
			"<configuration>"								    "\n"
			"	<runtime>"									    "\n"
			"		<loadFromRemoteSources enabled=\"true\" />" "\n"
			"	</runtime>"									    "\n"
			"</configuration>"								    "\n"
			));
		return true;
	}
	catch (...) {
	}

	return false;
}

static void DoTask()
{
	fs::path gExecutableDirectory = base::path::get(base::path::DIR_EXE).remove_filename();

	base::i18n::textdomain("YDWEStartup");
	base::i18n::bindtextdomain("YDWEStartup", gExecutableDirectory / L"share" / L"locale");

	if (gExecutableDirectory != fs::path(gExecutableDirectory.string()))
	{
		BOOST_THROW_EXCEPTION(std::domain_error(_("Error YDWE directory.")));
	}

	fs::path gWarcraftDirectory;
	if (!base::warcraft3::directory::get(__("Please choose your Warcraft 3 installation directory"), gWarcraftDirectory))
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

	CheckedCopyFile(gExecutableDirectory / L"bin" / "msvcp120.dll", gWarcraftDirectory / "msvcp120.dll");
	CheckedCopyFile(gExecutableDirectory / L"bin" / "msvcr120.dll", gWarcraftDirectory / "msvcr120.dll");

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

	CreateDotNetConfig(gWarcraftDirectory / L"worldeditydwe.exe.config");

	base::win::process worldedit_process;
	bool result = worldedit_process.create(worldeditPreferredPath, std::wstring(::GetCommandLineW()));

	if (!result)
	{
		throw base::windows_exception(_("Failed to launch world editor."));
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
	catch (base::exception const& e)
	{
		MessageBoxW(NULL, base::u2w(e.what(), base::conv_method::replace | '?').c_str(), __("Error"), MB_OK | MB_ICONERROR);
	}
	catch (std::exception const& e)
	{
		MessageBoxW(NULL, base::a2w(e.what(), base::conv_method::replace | '?').c_str(), __("Error"), MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBoxW(NULL, __("Unknown error"), __("Error"), MB_OK | MB_ICONERROR);
	}

	return exitCode;
}
