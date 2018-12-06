#include <windows.h>
#include "lua_engine/lua_loader.h"


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
	warcraft3::lua_engine::lua_loader::initialize();
}
