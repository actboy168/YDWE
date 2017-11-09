#include <windows.h>
#include "lua_engine/lua_loader.h"

namespace base { namespace warcraft3 { namespace lua_engine { namespace message {
	void set_window(HWND hwnd);
}}}}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID /*pReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);
	}

	return TRUE;
}

static HWND FindWar3Window()
{
	DWORD current_pid = GetCurrentProcessId();
	HWND hwnd = NULL;
	while (NULL != (hwnd = FindWindowExW(NULL, hwnd, L"Warcraft III", L"Warcraft III")))
	{
		DWORD pid = 0;
		GetWindowThreadProcessId(hwnd, &pid);
		if (pid == current_pid)
		{
			return hwnd;
		}
	}
	return NULL;
}

void Initialize()
{
	base::warcraft3::lua_engine::lua_loader::initialize();
	base::warcraft3::lua_engine::message::set_window(FindWar3Window());
}
