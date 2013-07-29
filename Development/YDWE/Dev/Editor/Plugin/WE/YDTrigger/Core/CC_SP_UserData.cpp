#include "CC_Include.h"

extern BOOL g_bDisableSaveLoadSystem;

VOID _fastcall
CC_Put_YDWESaveAnyTypeDataByUserData(DWORD This, DWORD OutClass, char* name)
{
  char buff[260];
  DWORD nKeyType   = GetVarType(This, 0);
  DWORD nValueType = GetVarType(This, 3);

  if ((CC_TYPE__begin < nKeyType) && (nKeyType < CC_TYPE__end)
    && (CC_TYPE__begin < nValueType) && (nValueType < CC_TYPE__end))
  {
    g_bDisableSaveLoadSystem = FALSE;

    BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerAny2I(%s, ", TypeName[nValueType], TypeName[nKeyType]);
    PUT_CONST(buff, 0);
    PUT_VAR(This, 1);
    BLZSStrPrintf(buff, 260, "), 0x%08X, ", SStrHash((char*)&GetGUIVar_Value(This, 2)));
    PUT_CONST(buff, 0);
    PUT_VAR(This, 4);
    PUT_CONST(")", 1);
  }
}

VOID _fastcall
CC_Put_YDWELoadAnyTypeDataByUserData(DWORD This, DWORD OutClass, char* name, char* type_name)
{
  char buff[260];
  DWORD nKeyType   = GetVarType(This, 0);

  if ((CC_TYPE__begin < nKeyType) && (nKeyType < CC_TYPE__end))
  {
    g_bDisableSaveLoadSystem = FALSE;

    BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerAny2I(%s, ", type_name, TypeName[nKeyType]);
    PUT_CONST(buff, 0);
    PUT_VAR(This, 1);
    BLZSStrPrintf(buff, 260, "), 0x%08X)", SStrHash((char*)&GetGUIVar_Value(This, 2)));
    PUT_CONST(buff, 0);
  }
}

VOID _fastcall
CC_Put_YDWEHaveSavedAnyTypeDataByUserData(DWORD This, DWORD OutClass, char* name)
{
  char buff[260];
  DWORD nKeyType   = GetVarType(This, 0);
  DWORD nValueType = GetVarType(This, 2);

  if ((CC_TYPE__begin < nKeyType) && (nKeyType < CC_TYPE__end)
    && (CC_TYPE__begin < nValueType) && (nValueType < CC_TYPE__end))
  {
    g_bDisableSaveLoadSystem = FALSE;

    BLZSStrPrintf(buff, 260, "YDTriggerHas(%s, YDTriggerAny2I(%s, ", TypeName[nValueType], TypeName[nKeyType]);
    PUT_CONST(buff, 0);
    PUT_VAR(This, 1);
    BLZSStrPrintf(buff, 260, "), 0x%08X)", SStrHash((char*)&GetGUIVar_Value(This, 3)));
    PUT_CONST(buff, 0);
  }
}

VOID _fastcall
CC_Put_YDWEFlushAnyTypeDataByUserData(DWORD This, DWORD OutClass, char* name)
{
  char buff[260];
  DWORD nKeyType   = GetVarType(This, 0);
  DWORD nValueType = GetVarType(This, 2);

  if ((CC_TYPE__begin < nKeyType) && (nKeyType < CC_TYPE__end)
    && (CC_TYPE__begin < nValueType) && (nValueType < CC_TYPE__end))
  {
    g_bDisableSaveLoadSystem = FALSE;

    BLZSStrPrintf(buff, 260, "call YDTriggerClear(%s, YDTriggerAny2I(%s, ", TypeName[nValueType], TypeName[nKeyType]);
    PUT_CONST(buff, 0);
    PUT_VAR(This, 1);
    BLZSStrPrintf(buff, 260, "), 0x%08X)", SStrHash((char*)&GetGUIVar_Value(This, 3)));
    PUT_CONST(buff, 1);
  }
}

VOID _fastcall
CC_Put_YDWEFlushAllByUserData(DWORD This, DWORD OutClass, char* name)
{
  char buff[260];
  DWORD nKeyType   = GetVarType(This, 0);

  if ((CC_TYPE__begin < nKeyType) && (nKeyType < CC_TYPE__end))
  {
    g_bDisableSaveLoadSystem = FALSE;

    BLZSStrPrintf(buff, 260, "call YDTriggerClearTable(YDTriggerAny2I(%s, ", TypeName[nKeyType]);
    PUT_CONST(buff, 0);
    PUT_VAR(This, 1);
    PUT_CONST("))", 1);
  }
}
