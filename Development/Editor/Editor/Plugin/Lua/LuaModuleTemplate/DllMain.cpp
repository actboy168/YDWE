#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}

int luaopen_<?php echo $argv[1]; ?>(lua_State *pState)
{
	return 0;
}
