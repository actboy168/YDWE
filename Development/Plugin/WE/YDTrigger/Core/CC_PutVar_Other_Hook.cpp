#include "CC_Include.h"
#include "locvar.h"

extern BOOL g_bYDWEEnumUnitsInRangeMultipleFlag;

int _fastcall 
CC_PutVar_Other_Hook(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD index, DWORD type)
{
  char buff[260];
  char NewName[260];
  DWORD nItemClass = *(DWORD*)(This+0x178);

  if ((0 != nItemClass) && (2 == *(DWORD*)(This+0x08)))
  {
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
      BLZSStrPrintf(NewName, 260, "%sFunc%03d", name, index+1);
	  return CC_PutVar(nItemClass, EDX, OutClass, NewName, 0, type, 1);
    case CC_GUIID_YDWEGetAnyTypeLocalVariable:
		locvar::get(nItemClass, OutClass, (char*)(This + 0x0C));
		// TODO
		return 1;
	case CC_GUIID_YDWEGetAnyTypeLocalArray:
		BLZSStrPrintf(NewName, 260, "%sFunc%03d", name, index + 1);
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
    default:
		{
			char szName[260];
			CC_GetGUIName(nItemClass, 0, szName, 260);
			if (locvar::trigger_data(nItemClass, OutClass, szName))
			{
				return 0;
			}
		}
      break;      
    }
  }
  return CC_PutVar_Other(This, EDX, OutClass, name, index, type);
}
