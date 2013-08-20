#include <windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include "Detours/detoured.h"
#include "Storm/StormAdapter.h"
#include "HashTable/GUIID_Table.h"
#include "Common.h"
#include "ConvertString/ConvertString.h"

int check()
{
  char path[MAX_PATH];
  int length;

  GetModuleFileNameA(NULL, path, MAX_PATH);

  if (path != NULL)
  {
    length = BLZSStrLen(path);
    return (0 == BLZSStrCmpI("worldeditydwe.exe", path+length-sizeof("worldeditydwe.exe")+1, MAX_PATH));
  }
  return FALSE;
}

CHAR g_szDllPath[MAX_PATH];
void SetDllPath(HMODULE hModule)
{
	if (NULL == GetModuleFileNameA(hModule, g_szDllPath, _countof(g_szDllPath)))
	{
		g_szDllPath[0] = '.';
		g_szDllPath[1] = '\0';
	}
	else
	{
		PathRemoveBackslashA(g_szDllPath);
		PathUnquoteSpacesA(g_szDllPath);
		PathRemoveFileSpecA(g_szDllPath);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  UNREFERENCED_PARAMETER(lpReserved);

  if (NULL == GetModuleHandleA("YDTrigger.dll"))
    return TRUE;
  if (!check())
    return TRUE;

  switch(ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
    {
      DisableThreadLibraryCalls(hModule);
      Detoured_Init(hModule); 
	  SetDllPath(hModule);
      GUIID_Table_Create();
      GUIID_Table_Init();
      All_Hook();
    }
    break;
  case DLL_PROCESS_DETACH:
  case DLL_THREAD_DETACH:
    {
      All_Unhook();  
      GUIID_Table_Destory();
    }
    break;
  }

  return TRUE;
}

const char *PluginName()
{
	return "YDTrigger";
}
