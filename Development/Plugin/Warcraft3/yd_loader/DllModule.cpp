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
#include <base/util/ini.h>
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

uintptr_t RealCreateWindowExA = 0;

FullWindowedMode fullWindowedMode;

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
}

void ResetConfig(base::ini::table& table)
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
	table["PJass"]["Option"] = "0";
	table["ScriptInjection"]["Option"] = "0";
	table["HostTest"]["Option"] = "0";
	table["ThirdPartyPlugin"]["EnableDotNetSupport"] = "1";
	table["ThirdPartyPlugin"]["EnableTesh"] = "1";
	table["ThirdPartyPlugin"]["EnableYDTrigger"] = "1";
	table["FeatureToggle"]["EnableManualNewId"] = "0";
	table["FeatureToggle"]["EnableTriggerCopyEncodingAutoConversion"] = "1";
	table["FeatureToggle"]["EnableShowInternalAttributeId"] = "0";
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
		base::ini::table table;
		ResetConfig(table);

		try {
			auto buf = base::file::read_stream(ydwe_path / L"bin" / L"EverConfig.cfg").read<std::string>();
			base::ini::read(table, buf.c_str());
		}
		catch (...) {
		}


		IsFullWindowedMode      = "0" != table["MapTest"]["LaunchFullWindowed"];
		IsLockingMouse          = "0" != table["MapTest"]["LaunchLockingMouse"];
		IsFixedRatioWindowed    = "0" != table["MapTest"]["LaunchFixedRatioWindowed"];
		IsWideScreenSupport     = "0" != table["MapTest"]["LaunchWideScreenSupport"];
		IsEnableDirect3D9       = "Direct3D 9" == table["MapTest"]["LaunchRenderingEngine"];
	} 
	catch (...) {
	}

	g_DllMod.hGameDll = GetModuleHandleW(L"Game.dll");

	if (g_DllMod.IsWideScreenSupport)
	{
		WideScreen::initialize();
	}

	RealCreateWindowExA = base::hook::iat(L"Game.dll", "user32.dll", "CreateWindowExA", (uintptr_t)FakeCreateWindowExA);

	auto_enter::game_status::initialize(g_DllMod.hGameDll);

	if (g_DllMod.IsEnableDirect3D9)
	{
		EnableDirect3D9(g_DllMod.hGameDll);
	}

	scores::rpg::hook();
}

void DllModule::Detach()
{
}
