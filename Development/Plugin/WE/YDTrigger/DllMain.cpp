#include <windows.h>
#include "HashTable/GUIID_Table.h"
#include "Common.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		::DisableThreadLibraryCalls(hModule);
		GUIID_Table_Create();
		GUIID_Table_Init();
		All_Hook();
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		All_Unhook();
		GUIID_Table_Destory();
		break;
	}
	return TRUE;
}
