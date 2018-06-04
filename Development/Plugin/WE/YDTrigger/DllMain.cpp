#include <windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

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
