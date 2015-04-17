#include <windows.h>
#include "lua_engine/lua_loader.h"

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID /*pReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);
		MessageBoxA(0, 0, 0, 0);
	}

	return TRUE;
}

void Initialize()
{
	base::warcraft3::lua_engine::lua_loader::initialize();
}

const char *PluginName()
{
	return "yd_lua_engine";
}
