#include "CC_Include.h"

extern BOOL g_local_in_mainproc;

extern BOOL g_bDisableSaveLoadSystem;

#define YDL_LOCALVAR_STEPS "ydl_localvar_step"

//
//  local integer ydl_localvar_step = YDWEGetIntegerByInteger(YDWEH2I(GetTriggeringTrigger()), StringHash("GlobalsTriggerRunSteps"))
//  set ydl_localvar_step = ydl_localvar_step + 3
//  call YDWESaveIntegerByInteger(YDWEH2I(GetTriggeringTrigger()), StringHash("GlobalsTriggerRunSteps"), ydl_localvar_step)
//  call YDWESaveIntegerByInteger(YDWEH2I(GetTriggeringTrigger()), StringHash("TriggerRunSteps"), ydl_localvar_step)
//
void _fastcall
CC_Put_TriggerLocalVar_Begin(DWORD OutClass)
{
  char buff[260];

  if (g_local_in_mainproc)
  {
    g_bDisableSaveLoadSystem = FALSE;

    CC_PutBegin();
    BLZSStrPrintf(buff, 260, "local integer "YDL_LOCALVAR_STEPS" = YDTriggerGetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X)", SStrHash("GlobalsTriggerRunSteps"));
    PUT_CONST(buff, 1);
    PUT_CONST("set "YDL_LOCALVAR_STEPS" = "YDL_LOCALVAR_STEPS" + 3", 1);
    BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X, "YDL_LOCALVAR_STEPS")", SStrHash("GlobalsTriggerRunSteps"));
    PUT_CONST(buff, 1);
    BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X, "YDL_LOCALVAR_STEPS")", SStrHash("TriggerRunSteps"));
    PUT_CONST(buff, 1);
    CC_PutEnd();
  }
}

//
//  call YDWEFlushMissionByInteger(YDWEH2I(GetTriggeringTrigger())*ydl_localvar_step)
//
void _fastcall 
CC_Put_TriggerLocalVar_End(DWORD OutClass)
{
  if (g_local_in_mainproc)
  {
    CC_PutBegin();
    PUT_CONST("call YDTriggerClearTable(YDTriggerH2I(GetTriggeringTrigger())*"YDL_LOCALVAR_STEPS")", 1);
    CC_PutEnd();
  }
}
