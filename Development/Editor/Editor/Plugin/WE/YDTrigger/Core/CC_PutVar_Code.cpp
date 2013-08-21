#include "CC_Include.h"

void _fastcall CC_PutActionEx_Hook(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD Type, DWORD Endl);

void _fastcall
CC_PutVar_Code(DWORD This, DWORD OutClass, char* name, DWORD index, DWORD Type)
{
  char NewName[260];
  DWORD VarClass = GetGUIVar_Class(This, index);

  if (VarClass != 0)
  {
    BLZSStrPrintf(NewName, 260, "%s%03d", name, index+1);
    if (Type == CC_GUI_TYPE_CONDITION)
    {
      CC_PutConditionEx(VarClass, OutClass, NewName);
    }
    else
    {
      CC_PutActionEx_Hook(VarClass, 0, OutClass, NewName, CC_GUI_TYPE_ACTION, 0);
    }
  }
}
