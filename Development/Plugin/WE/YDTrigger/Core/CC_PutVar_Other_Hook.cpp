#include "CC_Include.h"
#include "locvar.h"
#include <base/hook/fp_call.h>

extern BOOL g_bYDWEEnumUnitsInRangeMultipleFlag;

int _fastcall 
CC_PutVar_Other_Hook(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD index, DWORD type)
{
  char buff[260];
  char NewName[260];
  DWORD nItemClass = *(DWORD*)(This+0x178);

  if ((0 != nItemClass) && (2 == *(DWORD*)(This+0x08)))
  {
	  DWORD nVar = (*(DWORD*)(nItemClass + 0x128));
	  if (nVar == 3 && GetGUIVar_Type(nItemClass, 1) == CC_VARTYPE_ArithmeticOperator) {
		  BLZSStrPrintf(NewName, 260, "%s%03d", name, index + 1);
		  PUT_CONST("(", 0);
		  name = NewName;
		  PUT_VAR(nItemClass, 0);
		  PUT_CONST(" ", 0);
		  PUT_VAR(nItemClass, 1);
		  PUT_CONST(" ", 0);
		  PUT_VAR(nItemClass, 2);
		  return PUT_CONST(")", 0);
	  }
    switch (*(DWORD*)(nItemClass+0x138))
    {
    case CC_GUIID_GetEnumUnit:
        if (g_bYDWEEnumUnitsInRangeMultipleFlag)
        {
			return PUT_CONST(STRING_YDWE_LOCAL"unit", 0);
        }
        else
        {
			return PUT_CONST("GetEnumUnit()", 0);
        }
	case CC_GUIID_GetFilterUnit:
		if (g_bYDWEEnumUnitsInRangeMultipleFlag)
		{
			return PUT_CONST(STRING_YDWE_LOCAL"unit", 0);
		}
		else
		{
			return PUT_CONST("GetFilterUnit()", 0);
		}
    case CC_GUIID_YDWECustomScriptCode:
      BLZSStrPrintf(NewName, 260, "%s%03d", name, index+1);
	  return CC_PutVar(nItemClass, EDX, OutClass, NewName, 0, type, 1);
    case CC_GUIID_YDWEGetAnyTypeLocalVariable:
		locvar::get(nItemClass, OutClass, (char*)(This + 0x0C));
		// TODO
		return 1;
	case CC_GUIID_YDWEGetAnyTypeLocalArray:
		BLZSStrPrintf(NewName, 260, "%s%03d", name, index + 1);
		locvar::get_array(nItemClass, OutClass, NewName, (char*)(This+0x0C));
		// TODO
		return 1;
    case CC_GUIID_YDWELoadAnyTypeDataByUserData:
		return CC_Put_YDWELoadAnyTypeDataByUserData(nItemClass, OutClass, name, (char*)(This+0x0C));
    case CC_GUIID_YDWEHaveSavedAnyTypeDataByUserData:
		return CC_Put_YDWEHaveSavedAnyTypeDataByUserData(nItemClass, OutClass, name);
    case CC_GUIID_YDWEForLoopLocVarIndex:
      ConvertString((char*)&GetGUIVar_Value(nItemClass, 0), NewName, 260);
      BLZSStrPrintf(buff, 260, "ydul_%s", NewName);
	  return PUT_CONST(buff, 0);
	case CC_GUIID_GetTriggerName: {
		DWORD Trigger = *(DWORD*)(*(DWORD*)(This + 0x04) + 0x30);
		char szName[260];
		if (Trigger) {
			BLZSStrCopy(szName, (const char*)(Trigger + 0x4C), 260);
		}
		else {
			GetWEString("WESTRING_UNKNOWN", szName, 260, 0);
		}
		PUT_CONST("\"", 0);
		PUT_CONST(szName, 0);
		return PUT_CONST("\"", 0);
	}
	case CC_GUIID_OperatorString:
		BLZSStrPrintf(NewName, 260, "%s%03d", name, index + 1);
		PUT_CONST("(", 0);
		name = NewName;
		PUT_VAR(nItemClass, 0);
		PUT_CONST(" + ", 0);
		PUT_VAR(nItemClass, 1);
		return PUT_CONST(")", 0);
    default:
		{
			char szName[260];
			CC_GetGUIName(nItemClass, 0, szName, 260);
			if (locvar::trigger_data(nItemClass, OutClass, szName))
			{
				return 0;
			}
			PUT_CONST((char*)(nItemClass + 0x20), 0);
			PUT_CONST("(", 0);
			BLZSStrPrintf(NewName, 260, "%s%03d", name, index + 1);
			name = NewName;
			DWORD nVar = *(DWORD*)(nItemClass + 0x128);
			if (nVar > 0) {
				PUT_VAR(nItemClass, 0);
				for (DWORD i = 1; i < nVar; ++i) {
					PUT_CONST(", ", 0);
					PUT_VAR(nItemClass, i);
				}
			}
			return PUT_CONST(")", 0);
		}     
    }
  }
  else
  {
	  // TODO
	  base::fast_call<void>(0x0065B2A0, This, 0, buff, 260);
	  PUT_CONST(buff, 0);
	  if (!*(DWORD*)(This + 380))
	  {
		  return 0;
	  }
	  PUT_CONST("[", 0);
	  CC_PutVar_Other_Hook(*(DWORD*)(This + 0x17C), 0, OutClass, name, index, type);
	  return PUT_CONST("]", 0);
  }
}
