#include <windows.h>

BOOL APIENTRY DllMain(HMODULE /*module*/, DWORD reason, LPVOID /*pReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
	}

	return TRUE;
}
