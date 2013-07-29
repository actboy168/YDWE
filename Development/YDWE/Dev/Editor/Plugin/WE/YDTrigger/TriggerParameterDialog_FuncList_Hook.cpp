#include <windows.h>
#include "Storm/StormAdapter.h"
#include "Common.h"
#include "Core/CC_VarType.h"
#include "Hook.h"

APIInfo g_TPD_func_info;

DWORD g_nRet_TPD_func = 0;

int _fastcall
TPD_FuncList_Check(char* str1, char* str2)
{
  if (0 != BLZSStrCmp(str1, str2, 0x7FFFFFFF)) 
  {
    if (0 == BLZSStrCmp(str1, "AnyReturnType", 0x7FFFFFFF))
    {
      int i;
      for (i = CC_TYPE__begin+1; i < CC_TYPE__end; i++)
      {    
        if (0 == BLZSStrCmp(str2, TypeName[i], 0x7FFFFFFF))
        {
          return FALSE;
        }
      }
    }
    return TRUE;
  }
  return FALSE;
}

void __declspec(naked) TPD_FuncList_Hook()
{
  __asm
  {
    lea  edx, [esi+64h]
    mov  ecx, eax
    call TPD_FuncList_Check
    jmp [g_nRet_TPD_func]
  }
}

void
TriggerParameterDialog_FuncList_Hook()
{
  g_TPD_func_info.api_addr = (PBYTE)WE_ADDRESS(0x00688B0D);
  g_TPD_func_info.hook_func = TPD_FuncList_Hook;
  g_TPD_func_info.hook_mode = 0xE9;// jmp

  g_nRet_TPD_func = WE_ADDRESS(0x00688B1C);

  HookOne(&g_TPD_func_info);
}

void
TriggerParameterDialog_FuncList_Unhook()
{
  UnhookOne(&g_TPD_func_info);
}
