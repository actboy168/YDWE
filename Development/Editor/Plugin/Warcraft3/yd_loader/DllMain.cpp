#include <windows.h>
#include "DllModule.h"

DllModule g_DllMod;

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID /*pReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		MessageBoxA(0, 0, 0, 0);
		DisableThreadLibraryCalls(module);
		g_DllMod.Attach();
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		g_DllMod.Detach();
	}

	return TRUE;
}

const char *PluginName()
{
	return "yd_loader";
}
