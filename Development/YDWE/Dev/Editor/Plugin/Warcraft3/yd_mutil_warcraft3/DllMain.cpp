#include <windows.h>

void InitializeMutilWar3();

void Initialize()
{
	InitializeMutilWar3();
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID /*pReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}

const char *PluginName()
{
	return "yd_mutil_warcraft3";
}
