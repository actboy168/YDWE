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

void Initialize()
{
	base::warcraft3::lua_engine::lua_loader::initialize();
}

void SetWindow(HWND hwnd)
{
	base::warcraft3::lua_engine::message::set_window(hwnd);
}

const char *PluginName()
{
	return "yd_lua_engine";
}
