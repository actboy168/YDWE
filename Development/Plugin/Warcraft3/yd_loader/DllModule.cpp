#include "DllModule.h"
#include "FullWindowedMode.h"
#include <base/hook/fp_call.h>
#include <base/file/stream.h>
#include <base/hook/iat.h>
#include <base/hook/dyn_iat.h>
#include <base/path/helper.h>
#include <base/path/service.h>
#include <base/util/unicode.h>
#include <base/warcraft3/command_line.h>
#include <base/win/file_version.h>
#include <base/win/pe_reader.h>
#include <slk/reader/IniReader.hpp>
#include <slk/reader/IniReader.cpp>
#include <slk/reader/CommonReader.cpp>
#include <map>
#include "auto_enter.h"
#include "game_status.h"

namespace scores { namespace rpg {
	void hook();
}}

namespace WideScreen
{
	void initialize();
}

uintptr_t RealLoadLibraryA  = 0;
uintptr_t RealGameLoadLibraryA  = 0;
uintptr_t RealCreateWindowExA = 0;
uintptr_t RealSFileOpenArchive = (uintptr_t)::GetProcAddress(LoadLibraryW(L"Storm.dll"), (LPCSTR)266);

FullWindowedMode fullWindowedMode;

const char *PluginName();
void LockingMouse();

HWND WINAPI FakeCreateWindowExA(
	DWORD dwExStyle,
	LPCSTR lpClassName,
	LPCSTR lpWindowName,
	DWORD dwStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU hMenu,
	HANDLE hlnstance,
	LPVOID lpParam)
{
	HWND hWnd = base::std_call<HWND>(RealCreateWindowExA, dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hlnstance, lpParam);
	if ((0 == strcmp(lpClassName, "Warcraft III"))
		&& (0 == strcmp(lpWindowName, "Warcraft III"))
		&& (NULL == g_DllMod.hWar3Wnd))
	{
		g_DllMod.SetWindow(hWnd);
		if (g_DllMod.IsAuto)
		{
			auto_enter::initialize();
		}
		if (g_DllMod.IsFullWindowedMode)
		{
			fullWindowedMode.Start();
		}
		else
		{
			if (g_DllMod.IsFixedRatioWindowed)
			{
				RECT r;
				if (::GetWindowRect(hWnd, &r))
				{
					FullWindowedMode::FixedRatio(hWnd, r.right - r.left, r.bottom - r.top);
				}
			}
		}
		g_DllMod.ThreadStart();
	}

	return hWnd;
}

bool EnableDirect3D9(HMODULE gamedll)
{
	HMODULE d3dproxy = LoadLibraryW(L"d3d8proxy.dll");
	if (!d3dproxy)
	{
		return false;
	}

	uintptr_t Direct3DCreate8 = (uintptr_t)GetProcAddress(d3dproxy, "Direct3DCreate8");
	if (!Direct3DCreate8)
	{
		return false;
	}
	return base::hook::dyn_iat(gamedll, L"d3d8.dll", "Direct3DCreate8", 0, Direct3DCreate8);
}

HMODULE __stdcall FakeGameLoadLibraryA(LPCSTR lpFilePath)
{
	fs::path lib((const char*)lpFilePath);
	if (lib == "advapi32.dll")
	{
		return NULL;
	}

	return base::std_call<HMODULE>(RealGameLoadLibraryA, lpFilePath);
}

HMODULE __stdcall FakeLoadLibraryA(LPCSTR lpFilePath)
{
	fs::path lib((const char*)lpFilePath);
	if (lib == "Game.dll")
	{
		if (!g_DllMod.hGameDll)
		{
			g_DllMod.hGameDll = ::LoadLibraryW((g_DllMod.patch_path / L"Game.dll").c_str());
			if (!g_DllMod.hGameDll)
			{
				g_DllMod.hGameDll = ::LoadLibraryW(L"Game.dll");
				if (!g_DllMod.hGameDll)
				{
					return g_DllMod.hGameDll;
				}
			}

			if (g_DllMod.IsWideScreenSupport)
			{
				WideScreen::initialize();
			}

			RealCreateWindowExA  = base::hook::iat(L"Game.dll", "user32.dll", "CreateWindowExA", (uintptr_t)FakeCreateWindowExA);
			if (g_DllMod.IsDisableSecurityAccess)
			{
				RealGameLoadLibraryA = base::hook::iat(L"Game.dll", "kernel32.dll", "LoadLibraryA", (uintptr_t)FakeGameLoadLibraryA);
			}

			HANDLE hMpq;
			base::std_call<BOOL>(RealSFileOpenArchive, (g_DllMod.patch_path / "Patch.mpq").string().c_str(), 9, 6, &hMpq);

			g_DllMod.LoadPlugins();
			auto_enter::game_status::initialize(g_DllMod.hGameDll);

			if (g_DllMod.IsEnableDirect3D9)
			{
				EnableDirect3D9(g_DllMod.hGameDll);
			}

			scores::rpg::hook();

			return g_DllMod.hGameDll;
		}
	}

	return base::std_call<HMODULE>(RealLoadLibraryA, lpFilePath);
}

DllModule::DllModule()
	: hWar3Wnd(NULL)
	, IsWindowMode(false)
	, IsAuto(false)
	, IsFullWindowedMode(false)
	, IsLockingMouse(false)
	, IsFixedRatioWindowed(false)
	, IsEnableDirect3D9(false)
	, hGameDll(NULL)
	, daemon_thread_()
	, daemon_thread_exit_(false)
{ }



void DllModule::ThreadStart()
{
	daemon_thread_.reset(new base::thread(std::bind(&DllModule::ThreadFunc, this)));
}

void DllModule::ThreadStop()
{
	try {
		if (daemon_thread_)
		{
			daemon_thread_exit_ = true;
			daemon_thread_->join();
			daemon_thread_.reset();
		}
	} catch (...) { }
}

void DllModule::ThreadFunc()
{
	for (;!daemon_thread_exit_; base::this_thread::sleep_for(10))
	{
		LockingMouse();
	}
}

void DllModule::SetWindow(HWND hwnd)
{
	hWar3Wnd = hwnd;
	for (auto it = plugin_mapping.begin(); it != plugin_mapping.end(); ++it)
	{
		uintptr_t func = (uintptr_t)::GetProcAddress(it->second, "SetWindow");
		if (func)
		{
			base::c_call<void>(func, hwnd);
		}
	}
}

void DllModule::LoadPlugins()
{
	try {
		fs::path plugin_path = ydwe_path / L"plugin" / L"warcraft3";

		slk::IniTable table;
		try {
			base::buffer buf = base::file::read_stream(plugin_path / L"config.cfg").read<base::buffer>();
			base::buffer_reader reader(buf);
			slk::IniReader::Read(reader, table);
		}
		catch(...) {
		}

		fs::directory_iterator end_itr;
		for (fs::directory_iterator itr(plugin_path); itr != end_itr; ++itr)
		{
			try {
				if (!fs::is_directory(*itr))
				{
					std::string utf8_name = base::w2u(itr->path().filename().wstring());

					if ((! base::path::equal(itr->path().filename(), std::string(PluginName()) + ".dll"))
						&& base::path::equal(itr->path().extension(), L".dll")
						&& ("0" != table["Enable"][utf8_name]))
					{
						HMODULE module = ::LoadLibraryW(itr->path().c_str());
						if (module)
						{
							plugin_mapping.insert(std::make_pair(utf8_name, module));
						}
					}
				}
			}
			catch(...) {
			}
		}

		for (auto it = plugin_mapping.begin(); it != plugin_mapping.end(); )
		{
			HMODULE            module = it->second;
			uintptr_t fPluginName = (uintptr_t)::GetProcAddress(module, "PluginName");
			uintptr_t fInitialize = (uintptr_t)::GetProcAddress(module, "Initialize");
			if (fPluginName 
				&& fInitialize
				&& (it->first == std::string(base::c_call<const char *>(fPluginName)) + ".dll"))
			{
				++it;
			}
			else
			{
				::FreeLibrary(module);
				plugin_mapping.erase(it++);
			}
		}

		for (auto it = plugin_mapping.begin(); it != plugin_mapping.end(); ++it)
		{
			base::c_call<void>(::GetProcAddress(it->second, "Initialize"));
		}
	}
	catch(...) {
	}
}

void ResetConfig(slk::IniTable& table)
{
	table["MapSave"]["Option"] = "0";
	table["War3Patch"]["Option"] = "0";
	table["MapTest"]["LaunchRenderingEngine"] = "Direct3D 8";
	table["MapTest"]["LaunchWindowed"] = "1";
	table["MapTest"]["LaunchFullWindowed"] = "0";
	table["MapTest"]["LaunchWideScreenSupport"] = "1";
	table["MapTest"]["LaunchLockingMouse"] = "0";
	table["MapTest"]["LaunchFixedRatioWindowed"] = "0";
	table["MapTest"]["LaunchDisableSecurityAccess"] = "0";	
	table["ScriptCompiler"]["EnableJassHelper"] = "1";
	table["ScriptCompiler"]["EnableJassHelperDebug"] = "0";
	table["ScriptCompiler"]["EnableJassHelperOptimization"] = "1";
	table["ScriptCompiler"]["EnableCJass"] = "0";
	table["ScriptInjection"]["Option"] = "0";
	table["HostTest"]["Option"] = "0";
	table["ThirdPartyPlugin"]["EnableDotNetSupport"] = "1";
	table["ThirdPartyPlugin"]["EnableTesh"] = "1";
	table["ThirdPartyPlugin"]["EnableYDTrigger"] = "1";
	table["FeatureToggle"]["EnableManualNewId"] = "0";
	table["FeatureToggle"]["EnableTriggerCopyEncodingAutoConversion"] = "1";
	table["FeatureToggle"]["EnableShowInternalAttributeId"] = "0";
}

bool DllModule::SearchPatch(fs::path& result, std::wstring const& fv_str)
{
	try {
		fs::directory_iterator end_itr;
		for (fs::directory_iterator itr(ydwe_path / L"share" / L"patch"); itr != end_itr; ++itr)
		{
			try {
				if (fs::is_directory(*itr))
				{
					if (fs::exists(*itr / "Game.dll") && fs::exists(*itr / "Patch.mpq"))
					{
						base::win::file_version fv((*itr / "Game.dll").c_str());
						if (fv_str == fv[L"FileVersion"])
						{
							result = *itr;
							return true;
						}
					}
				}
			}
			catch(...) {
			}
		}
	}
	catch(...) {
	}

	return false;
}

void DllModule::Attach()
{
	base::warcraft3::command_line::parse([&](std::wstring const& key, std::wstring const& val){
		if (key == L"ydwe")
		{
			ydwe_path = val;
		}
		else if (key == L"window")
		{
			IsWindowMode = true;
		}
		else if (key == L"auto")
		{
			IsAuto = true;
		}
	});

	try {
		slk::IniTable table;
		ResetConfig(table);

		try {
			base::buffer buf = base::file::read_stream(ydwe_path / L"bin" / L"EverConfig.cfg").read<base::buffer>();
			base::buffer_reader reader(buf);
			slk::IniReader::Read(reader, table);
		} 
		catch (...) {
		}

		IsFullWindowedMode      = "0" != table["MapTest"]["LaunchFullWindowed"];
		IsLockingMouse          = "0" != table["MapTest"]["LaunchLockingMouse"];
		IsFixedRatioWindowed    = "0" != table["MapTest"]["LaunchFixedRatioWindowed"];
		IsWideScreenSupport     = "0" != table["MapTest"]["LaunchWideScreenSupport"];
		IsDisableSecurityAccess = "0" != table["MapTest"]["LaunchDisableSecurityAccess"];
		IsEnableDirect3D9       = "Direct3D 9" == table["MapTest"]["LaunchRenderingEngine"];

		try {
			if (table["War3Patch"]["Option"] == "1")
			{
				if (table["MapSave"]["Option"] == "1")
				{
					SearchPatch(patch_path, L"1.20.4.6074");
				}
				else if (table["MapSave"]["Option"] == "2")
				{
					SearchPatch(patch_path, L"1.24.4.6387");
				}
			}
			else if (table["War3Patch"]["Option"] == "2")
			{
				patch_path = ydwe_path / L"share" / L"patch"/ table["War3Patch"]["DirName"];
			}
		}
		catch (...) {
		}
	} 
	catch (...) {
	}

	RealLoadLibraryA  = base::hook::iat(L"War3.exe", "kernel32.dll", "LoadLibraryA", (uintptr_t)FakeLoadLibraryA);
}

void DllModule::Detach()
{
}
