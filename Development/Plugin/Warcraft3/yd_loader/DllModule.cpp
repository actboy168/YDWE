#include "DllModule.h"
#include <base/hook/fp_call.h>
#include <base/file/stream.h>
#include <base/hook/iat.h>
#include <base/hook/dyn_iat.h>
#include <base/path/helper.h>
#include <bee/utility/unicode.h>
#include <base/warcraft3/command_line.h>
#include <base/win/pe_reader.h>
#include <base/util/ini.h>
#include <map>
#include "auto_enter.h"
#include "game_status.h"

namespace scores { namespace rpg {
	void hook();
}}

uintptr_t RealCreateWindowExA = 0;

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
	}

	return hWnd;
}


DllModule::DllModule()
	: hWar3Wnd(NULL)
	, IsAuto(false)
	, hGameDll(NULL)
{ }

void DllModule::SetWindow(HWND hwnd)
{
	hWar3Wnd = hwnd;
}

void DllModule::Attach()
{
	base::warcraft3::command_line::parse([&](std::wstring const& key, std::wstring const&){
		if (key == L"auto")
		{
			IsAuto = true;
		}
	});

	g_DllMod.hGameDll = GetModuleHandleW(L"Game.dll");

	RealCreateWindowExA = base::hook::iat(L"Game.dll", "user32.dll", "CreateWindowExA", (uintptr_t)FakeCreateWindowExA);

	auto_enter::game_status::initialize(g_DllMod.hGameDll);

	scores::rpg::hook();
}

void DllModule::Detach()
{
}
