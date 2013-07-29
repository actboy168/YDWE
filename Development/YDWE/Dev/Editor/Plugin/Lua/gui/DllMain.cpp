#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <windows.h>

HMODULE self_module = NULL;

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
		self_module = module;
	}

	return TRUE;
}
