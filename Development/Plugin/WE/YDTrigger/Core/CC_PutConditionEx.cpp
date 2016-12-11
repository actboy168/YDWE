#include "CC_Include.h"

int _fastcall
CC_FindVarType(DWORD This, DWORD VarId)
{
  DWORD nVar, i;
    
  nVar = *(DWORD*)(This+0x128);

  for (i = 0; i < nVar; i++)
  {
    if (VarId == GetGUIVar_Type(This, i))
      return i;
  }
  
  return -1;
}

void _fastcall
CC_PutConditionEx(DWORD This, DWORD OutClass, char* name)
{
  char GUI_Name[260];
  DWORD Index, Id, nVar, i;

  Index = CC_FindVarType(This, CC_VARTYPE_ComparisonOperator);
  if (Index != -1)
  {
    PUT_CONST("(", 0);
    PUT_VAR(This, Index-1);
    PUT_CONST(" ", 0);
    PUT_VAR(This, Index);
    PUT_CONST(" ", 0);
    PUT_VAR(This, Index+1);
    PUT_CONST(")", 0);
    return;
  }

  Index = CC_FindVarType(This, CC_VARTYPE_EqualNotEqualOperator);
  if (Index != -1)
  {
    PUT_CONST("(", 0);
    PUT_VAR(This, Index-1);
    PUT_CONST(" ", 0);
    PUT_VAR(This, Index);
    PUT_CONST(" ", 0);
    PUT_VAR(This, Index+1);
    PUT_CONST(")", 0);
    return;
  }
  
  Id = *(DWORD*)(This+0x138);
  switch (Id)
  {
  case CC_GUIID_TestTriggerConditions:
    PUT_CONST("true", 0);
    return;

  case CC_GUIID_GetBooleanAnd:
    PUT_CONST("(", 0);
    CC_PutVar_Code(This, OutClass, name, 0, CC_GUI_TYPE_CONDITION);
    PUT_CONST(" and ", 0);
    CC_PutVar_Code(This, OutClass, name, 1, CC_GUI_TYPE_CONDITION);
    PUT_CONST(")", 0);
    return;

  case CC_GUIID_GetBooleanOr:
    PUT_CONST("(", 0);
    CC_PutVar_Code(This, OutClass, name, 0, CC_GUI_TYPE_CONDITION);
    PUT_CONST(" or ", 0);
    CC_PutVar_Code(This, OutClass, name, 1, CC_GUI_TYPE_CONDITION);
    PUT_CONST(")", 0);
    return;

  case CC_GUIID_AndMultiple:
    PUT_CONST("(", 0);
    CC_PutBlock_Condition_And(This, OutClass, name, 0);
    PUT_CONST(")", 0);
    return;

  case CC_GUIID_OrMultiple:
    PUT_CONST("(", 0);
    CC_PutBlock_Condition_Or(This, OutClass, name, 0);
    PUT_CONST(")", 0);
    return;
  }

  if (Id != 1)
  {
    CC_GetGUIName(This, 0, GUI_Name, 260);
    PUT_CONST(GUI_Name, 0);
    PUT_CONST("(", 0);
  }
  
  nVar = *(DWORD*)(This+0x128);
  if (nVar > 0)
  {
    PUT_VAR(This, 0);
    for (i = 1; i < nVar; i++)
    {
      PUT_CONST(", ", 0);
      PUT_VAR(This, i);
    }
  }

  if (Id != 1)
  {
    PUT_CONST(")", 0);
  }

  return;
}


void _fastcall
CC_PutCondition(DWORD This, DWORD OutClass, char* name, DWORD index)
{
  char NewName[260];
  BLZSStrPrintf(NewName, 260, "%sFunc%03d", name, index+1);

  CC_PutConditionEx(This, OutClass, NewName);
}

