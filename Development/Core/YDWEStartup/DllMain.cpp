#include <list>
#include <utility>
#include <locale>
#include <windows.h>
#include <base/filesystem.h>
#include <base/file/memory_mapped_file.h>
#include <bee/error.h>
#include <base/file/stream.h>
#include <base/i18n/gettext.h>
#include <base/path/get_path.h>
#include <base/path/ydwe.h>
#include <bee/utility/unicode.h>
#include <base/win/file_version.h>
#include <bee/utility/format.h>	 
#include <base/util/ini.h>
#include <bee/subprocess.h>
#include "Splash.h"

#define YDWE_WAR3_INLINE
#include <base/warcraft3/directory.h>
#include <base/warcraft3/directory.cpp>
#include <base/warcraft3/command_line.h>
#include <base/warcraft3/command_line.cpp>

#define _(str)  base::i18n::get_text(str).data()
#define __(str) bee::u2w(base::i18n::get_text(str)).c_str()

static bool FileContentEqual(const fs::path &fileFirst, const fs::path &fileSecond, std::error_code *pErrorCode = nullptr)
{
	try
	{
		if (pErrorCode)
		{
			pErrorCode->assign(0, std::generic_category());
		}

		base::file::memory_mapped_file mapperFirst(fileFirst.c_str());
		base::file::memory_mapped_file mapperSecond(fileSecond.c_str());

		size_t size;
		return ((size = mapperFirst.size()) == mapperSecond.size())
			&& (memcmp(mapperFirst.data(), mapperSecond.data(), size) == 0);
	}
	catch (std::system_error const& e)
	{
		if (pErrorCode)
		{
			*pErrorCode = e.code();
		}

		return false;
	}
}

bool Blp2Bmp(const wchar_t* blp, const wchar_t* bmp);
bool Bmp2Blp(const wchar_t* bmp, const wchar_t* blp);

static void ShowSplash(fs::path const& ydwe_path)
{
	fs::path bmp = ydwe_path / L"bin" / L"splash.bmp";
	if (!fs::exists(bmp))
		return;

	try {
		base::ini::table table;
		fs::path dir = ydwe_path / L"share" / "zh-CN" / L"mpq" / "ReplaceableTextures" / "WorldEditUI";
		fs::create_directories(dir);
		fs::path blp1 = dir / "WarcraftIIILogo.blp";
		if (!fs::exists(blp1))
		{
			Bmp2Blp(bmp.wstring().c_str(), blp1.wstring().c_str());
		}
		fs::path blp2 = dir / "WarcraftIIIFTLogo.blp";
		if (!fs::exists(blp2))
		{
			fs::copy_file(blp1, blp2);
		}
	}
	catch (...) {
	}

	try {
		base::win::simple_file_version fv((ydwe_path / "YDWE.exe").c_str());
		CSplash display;
		display.SetBitmap(bmp.wstring().c_str());
		display.SetTransparentColor(RGB(128, 128, 128));
		display.SetText(bee::format(L"%d.%d.%d.%d", fv.major, fv.minor, fv.revision, fv.build).c_str(), 10, 10, 8, 16);
		display.Show();
		Sleep(5000);
		display.Close();
	}
	catch (...) {
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
#if _MSC_VER >= 1910
	fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
#else
	fs::copy_file(source, destination, fs::copy_option::overwrite_if_exists);
#endif
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
	fs::path gExecutableDirectory = base::path::ydwe(false);

	base::i18n::initialize(base::path::ydwe(true) / L"share" / L"locale");
	base::i18n::set_domain(L"startup");

	if (gExecutableDirectory != fs::path(gExecutableDirectory.string()))
	{
		throw std::domain_error(_("ERROR_YDWE_DIR"));
	}

	fs::path gWarcraftDirectory;
	if (!base::warcraft3::directory::get(__("CHOOSE_WAR3_DIR"), gWarcraftDirectory))
	{
		return ;
	}

	// Set current directory
	SetCurrentDirectoryW(gExecutableDirectory.c_str());

	if (fs::exists(gWarcraftDirectory / L"YDDllFixer.dll"))
	{
		if (!fs::remove(gWarcraftDirectory / L"YDDllFixer.dll"))
		{
			throw std::domain_error(_("ERROR_DEL_YDDLLFIXER"));
		}
	}

	if (fs::exists(gExecutableDirectory / L"YDDllFixer.dll"))
	{
		if (!fs::remove(gExecutableDirectory / L"YDDllFixer.dll"))
		{
			throw std::domain_error(_("ERROR_DEL_YDDLLFIXER"));
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
		throw std::domain_error(_("ERROR_WE_EXE"));
	}

	CreateDotNetConfig(gWarcraftDirectory / L"worldeditydwe.exe.config");

	SetEnvironmentVariableW(L"ydwe-process-name", L"ydwe");
	bee::subprocess::spawn worldedit_process;

	int argc = 0;
	wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
	std::vector<std::wstring> args(argc);
	for (int i = 0; i < argc; ++i) {
		args[i] = argv[i];
	}
	args[0] = worldeditPreferredPath.wstring();

	if (!worldedit_process.exec(args, 0)) {
		throw bee::make_syserror(_("ERROR_LAUNCH_WE"));
	}
	ShowSplash(gExecutableDirectory);
}

INT WINAPI YDWEStartup(HINSTANCE current, HINSTANCE previous, LPSTR pCommandLine, INT showType)
{
	base::warcraft3::command_line cmd;

	if (cmd.has(L"war3"))
	{
		bool launch_warcraft3(base::warcraft3::command_line&);
		launch_warcraft3(cmd);
		return 0;
	}

	INT exitCode = -1;

	try
	{
		DoTask();
		exitCode = 0;
	}
	catch (std::system_error const& e)
	{
		MessageBoxW(NULL, bee::u2w(e.what()).c_str(), __("ERROR"), MB_OK | MB_ICONERROR);
	}
	catch (std::exception const& e)
	{
		MessageBoxW(NULL, bee::a2w(e.what()).c_str(), __("ERROR"), MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBoxW(NULL, __("ERROR_UNKNOWN"), __("ERROR"), MB_OK | MB_ICONERROR);
	}

	return exitCode;
}
