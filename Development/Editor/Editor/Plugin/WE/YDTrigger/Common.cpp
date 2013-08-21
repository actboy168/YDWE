#include <windows.h>
#include <detours.h>
#include "Detours/detoured.h"
#include "Storm/StormAdapter.h"
#include "Common.h"

void _fastcall CC_PutTrigger_Hook(DWORD This, DWORD EDX, DWORD OutClass);
void _fastcall CC_PutVar_Other_Hook(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD index, DWORD type);
int  _fastcall GetGUICount_Hook(DWORD This);
int  _fastcall GetGUIString_Hook(DWORD This, DWORD EDX, int index, char* buff, int len);
int  _fastcall GetGUIIcon_Hook(DWORD This, DWORD EDX, int index);
int  _fastcall SetGUIId_Hook(DWORD This, DWORD EDX, char* name);
void _fastcall ConvertTriggerName_Hook(char* src, char* dst, int len);
void _fastcall GetGlobalVarName_Hook(DWORD This, DWORD EDX, DWORD index, char* dst, int len);
void _fastcall ChangeGUIType_Hook(DWORD This, DWORD EDX, DWORD unknow);
void _fastcall CC_Put_globals_Hook(DWORD OutClass);
void _fastcall CC_Put_endglobals_Hook(DWORD OutClass);
void _fastcall CC_Main_Hook(DWORD OutClass);

DWORD g_nWEBase = 0x00000000;

void Hook_Init()
{
  g_nWEBase = (DWORD)GetModuleHandle(NULL);
                                                         
  CC_PutTrigger           = (CC_PutTriggerProc)          WE_ADDRESS(0x005CA4C0);
  CC_PutVar_Other         = (CC_PutVar_OtherProc)        WE_ADDRESS(0x0065B5F0);
  ConvertTriggerName      = (ConvertTriggerNameProc)     WE_ADDRESS(0x005A4A80);
  GetGlobalVarName        = (GetGlobalVarNameProc)       WE_ADDRESS(0x005C6750);
  GetGUICount             = (GetGUICountProc)            WE_ADDRESS(0x005DAE20);
  GetGUIString            = (GetGUIStringProc)           WE_ADDRESS(0x005DAEE0);
  GetGUIIcon              = (GetGUIIconProc)             WE_ADDRESS(0x005DAE70);
  SetGUIId                = (SetGUIIdProc)               WE_ADDRESS(0x005D72F0);
  ChangeGUIType           = (ChangeGUITypeProc)          WE_ADDRESS(0x005D82C0);
  CC_Put_globals          = (CC_Put_globalsProc)         WE_ADDRESS(0x005BC680);
  CC_Put_endglobals       = (CC_Put_endglobalsProc)      WE_ADDRESS(0x005BC6A0);
  CC_Main                 = (CC_MainProc)                WE_ADDRESS(0x005CEE70);
  
  GetTriggerName_InitTrig = (GetTriggerName_InitTrigProc)WE_ADDRESS(0x005C7FF0);
  GetTriggerVar           = (GetTriggerVarProc)          WE_ADDRESS(0x005C8110);
  CC_GetGUIName           = (CC_GetGUINameProc)          WE_ADDRESS(0x005D7270);
  CC_PutBegin             = (CC_VoidProc)                WE_ADDRESS(0x004D3420);
  CC_PutEnd               = (CC_VoidProc)                WE_ADDRESS(0x004D3430);
  CC_PutConst             = (CC_PutConstProc)            WE_ADDRESS(0x004D3440);
  CC_PutString            = (CC_PutStringProc)           WE_ADDRESS(0x005BCB10);
  CC_PutVar               = (CC_PutVarProc)              WE_ADDRESS(0x005DA6A0);
  CC_PutEventRegister     = (CC_PutEventRegisterProc)    WE_ADDRESS(0x005DAA20);
  GetWEString             = (GetWEStringProc)            WE_ADDRESS(0x004EEC00);
  SetGUIUnknow            = (SetGUIUnknowProc)           WE_ADDRESS(0x005D7800);
  CC_PutActionEx          = (CC_PutActionExProc)         WE_ADDRESS(0x005DAFB0);
  SetGUIType              = (SetGUITypeProc)             WE_ADDRESS(0x005D7C00);
  CC_GetGlobalVar         = (CC_GetGlobalVarProc)        WE_ADDRESS(0x0065B520);
}

void TriggerParameterDialog_FuncList_Hook();
void TriggerParameterDialog_FuncList_Unhook();

void TriggerParameterDialog_Type_Hook();
void TriggerParameterDialog_Type_Unhook();

void All_Hook()
{
  Hook_Init();

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
    DetourAttach((PVOID*)&CC_PutTrigger, CC_PutTrigger_Hook);
    DetourAttach((PVOID*)&CC_PutVar_Other, CC_PutVar_Other_Hook);
    DetourAttach((PVOID*)&GetGUICount, GetGUICount_Hook);
    DetourAttach((PVOID*)&GetGUIString, GetGUIString_Hook);
    DetourAttach((PVOID*)&GetGUIIcon, GetGUIIcon_Hook);
    DetourAttach((PVOID*)&SetGUIId, SetGUIId_Hook);
    DetourAttach((PVOID*)&CC_Main, CC_Main_Hook);
    DetourAttach((PVOID*)&CC_Put_globals, CC_Put_globals_Hook);
    DetourAttach((PVOID*)&CC_Put_endglobals, CC_Put_endglobals_Hook);
    //DetourAttach((PVOID*)&ConvertTriggerName, ConvertTriggerName_Hook);
    //DetourAttach((PVOID*)&GetGlobalVarName, GetGlobalVarName_Hook);
    DetourAttach((PVOID*)&ChangeGUIType, ChangeGUIType_Hook);
  
    TriggerParameterDialog_FuncList_Hook();
	TriggerParameterDialog_Type_Hook();
  DetourTransactionCommit();
}

void All_Unhook()
{ 
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
    DetourDetach((PVOID*)&CC_PutTrigger, CC_PutTrigger_Hook);
    DetourDetach((PVOID*)&CC_PutVar_Other, CC_PutVar_Other_Hook);
    DetourDetach((PVOID*)&GetGUICount, GetGUICount_Hook);
    DetourDetach((PVOID*)&GetGUIString, GetGUIString_Hook);
    DetourDetach((PVOID*)&GetGUIIcon, GetGUIIcon_Hook);
    DetourDetach((PVOID*)&SetGUIId, SetGUIId_Hook);
    DetourDetach((PVOID*)&CC_Main, CC_Main_Hook);
    DetourDetach((PVOID*)&CC_Put_globals, CC_Put_globals_Hook);
    DetourDetach((PVOID*)&CC_Put_endglobals, CC_Put_endglobals_Hook);
    //DetourDetach(&(PVOID)ConvertTriggerName, ConvertTriggerName_Hook);
    //DetourDetach((PVOID*)&GetGlobalVarName, GetGlobalVarName_Hook);
    DetourDetach((PVOID*)&ChangeGUIType, ChangeGUIType_Hook);

    TriggerParameterDialog_FuncList_Unhook();
	TriggerParameterDialog_Type_Unhook();
  DetourTransactionCommit();
}

// Hook global var
CC_VoidProc                 CC_PutBegin;
CC_VoidProc                 CC_PutEnd;
CC_PutConstProc             CC_PutConst;
CC_PutVarProc               CC_PutVar;
CC_PutEventRegisterProc     CC_PutEventRegister;
CC_GetGUINameProc           CC_GetGUIName;
GetTriggerVarProc           GetTriggerVar;
GetTriggerName_InitTrigProc GetTriggerName_InitTrig;
GetWEStringProc             GetWEString;
SetGUIUnknowProc            SetGUIUnknow;
CC_PutActionExProc          CC_PutActionEx;
SetGUITypeProc              SetGUIType;
CC_GetGlobalVarProc         CC_GetGlobalVar;
CC_PutStringProc            CC_PutString;

CC_PutTriggerProc           CC_PutTrigger;
CC_PutVar_OtherProc         CC_PutVar_Other;
GetGUICountProc             GetGUICount;
GetGUIStringProc            GetGUIString;
GetGUIIconProc              GetGUIIcon;
SetGUIIdProc                SetGUIId;
ConvertTriggerNameProc      ConvertTriggerName;
GetGlobalVarNameProc        GetGlobalVarName;
ChangeGUITypeProc           ChangeGUIType;
CC_Put_globalsProc          CC_Put_globals;
CC_Put_endglobalsProc       CC_Put_endglobals;
CC_MainProc                 CC_Main;
