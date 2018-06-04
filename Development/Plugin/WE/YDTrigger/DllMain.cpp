#include <windows.h>
#include "Common.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		::DisableThreadLibraryCalls(hModule);
		All_Hook();
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		All_Unhook();
		break;
	}
	return TRUE;
}
