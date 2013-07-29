#include <windows.h>
#include "Detours/detoured.h"
#include "Common.h"

#ifdef CONFIG_TestMode

#define WE_ADDRESS(addr) ((DWORD)(addr)-0x00400000+(DWORD)GetModuleHandle(NULL))

typedef int (_fastcall* ReadUIDataProc)(DWORD Class, char* filepath, DWORD Unknow);
ReadUIDataProc ReadUIData;
DWORD* g_UIData_Table;

typedef int (_fastcall* ReadAllUIProc)();
ReadAllUIProc ReadAllUI;

void ReadUI_Init()
{
  ReadUIData = (ReadUIDataProc)WE_ADDRESS(0x00409EA0);
  ReadAllUI  = (ReadAllUIProc) WE_ADDRESS(0x004D1CA0);
  g_UIData_Table = (DWORD*)    WE_ADDRESS(0x00820004);
}

int _fastcall
ReadAllUI_Hook()
{
  DWORD nRet = ReadAllUI();

  ReadUIData(*g_UIData_Table, "UI\\TestData.txt", 0);

  return nRet;
}

void ReadUIHook()
{
  ReadUI_Init();

  Detoured_Hook(&(PVOID)ReadAllUI, ReadAllUI_Hook);
}

void ReadUIUnhook()
{
  Detoured_Unhook(&(PVOID)ReadAllUI, ReadAllUI_Hook);
}

#undef WE_ADDRESS

#endif