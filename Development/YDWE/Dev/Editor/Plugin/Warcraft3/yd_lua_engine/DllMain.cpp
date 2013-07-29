#include <windows.h>
#include "main/lua_loader.h"

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);
	}

	return TRUE;
}

#include <ydwe/util/console.h>

void Initialize()
{
	ydwe::util::console::enable();
	ydwe::warcraft3::lua_engine::lua_loader::initialize();
}

const char *PluginName()
{
	return "yd_lua_engine";
}
