#include <Windows.h>
#include "YDWEHook.h"

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		NYDWE::UninstallHooks();
	}

	return TRUE;
}
