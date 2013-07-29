#include <windows.h>

void HookAttach();
void HookDetach();

void Initialize()
{
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
		HookAttach();
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		HookDetach();
	}

	return TRUE;
}

const char *PluginName()
{
	return "yd_net_proxy";
}
