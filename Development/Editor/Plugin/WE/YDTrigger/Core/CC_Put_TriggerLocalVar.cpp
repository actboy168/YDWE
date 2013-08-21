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

int _fastcall Utf8toAscii(char src[], char dst[], unsigned int limit);

//
//  YDWEGetTypenameByInteger(YDWEH2I(GetTriggeringTrigger())*YDWEGetIntegerByInteger(YDWEH2I(GetTriggeringTrigger()), StringHash("TriggerRunSteps")), name)
//  YDWEGetTypenameByInteger(YDWEH2I(GetTriggeringTrigger())*ydl_localvar_step, name)
//
void _fastcall 
CC_Put_TriggerLocalVar_Get(DWORD This, DWORD OutClass, char* type_name)
{
  char buff[260];
  LPCSTR lpszKey;

  g_bDisableSaveLoadSystem = FALSE;

  lpszKey = (LPCSTR)&GetGUIVar_Value(This, 0);
  if (!SaveLoadCheck_Set(lpszKey, (LPCSTR)type_name)) 
  {
	  char tmp[260];
	  Utf8toAscii((char*)lpszKey, tmp, 260);
	  ShowErrorN(OutClass, "WESTRING_ERROR_YDTRIGGER_LOCVAR", tmp, (LPCSTR)type_name, tmp, SaveLoadCheck_Get(lpszKey));
  }

  if (g_local_in_mainproc)
  {
    BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*"YDL_LOCALVAR_STEPS", 0x%08X)", type_name, SStrHash(lpszKey));
  }
  else
  {
    BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*YDTriggerGetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X), 0x%08X)", type_name, SStrHash("TriggerRunSteps"), SStrHash(lpszKey));
  }
  PUT_CONST(buff, 0);
}

//
//  call YDWESaveTypenameByInteger(YDWEH2I(GetTriggeringTrigger())*YDWEGetIntegerByInteger(YDWEH2I(GetTriggeringTrigger()), StringHash("TriggerRunSteps")), name, value)   
//  call YDWESaveTypenameByInteger(YDWEH2I(GetTriggeringTrigger())*ydl_localvar_step, name, value)
//
void _fastcall
CC_Put_TriggerLocalVar_Set(DWORD This, DWORD OutClass, char* name)
{
  char buff[260];
  int var_type = GetVarType(This, 0);
  LPCSTR lpszKey;

  if ((CC_TYPE__begin < var_type) && (var_type < CC_TYPE__end))
  {
    g_bDisableSaveLoadSystem = FALSE;

	lpszKey = (LPCSTR)&GetGUIVar_Value(This, 1);
	if (!SaveLoadCheck_Set(lpszKey, TypeName[var_type]))
	{
		char tmp[260];
		Utf8toAscii((char*)lpszKey, tmp, 260);
		ShowErrorN(OutClass, "WESTRING_ERROR_YDTRIGGER_LOCVAR", tmp, TypeName[var_type], tmp, SaveLoadCheck_Get(lpszKey));
	}

    if (g_local_in_mainproc)
    {
      BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*"YDL_LOCALVAR_STEPS", 0x%08X, ", TypeName[var_type], SStrHash(lpszKey));
    }
    else
    {
      BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*YDTriggerGetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X), 0x%08X, ", TypeName[var_type], SStrHash("TriggerRunSteps"), SStrHash(lpszKey));
    }
    PUT_CONST(buff, 0);
    PUT_VAR(This, 2);
    PUT_CONST(")", 1);
  }
}

//
//  call YDWESaveIntegerByInteger(YDWEH2I(GetTriggeringTrigger()), StringHash("TriggerRunSteps"), ydl_localvar_step)
//
void _fastcall
CC_Put_TriggerLocalVar_Sleep_End(DWORD OutClass)
{
  char buff[260];


  if (g_local_in_mainproc)
  {
    g_bDisableSaveLoadSystem = FALSE;

    BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X, "YDL_LOCALVAR_STEPS")", SStrHash("TriggerRunSteps"));
    PUT_CONST(buff, 1);
  }
}
