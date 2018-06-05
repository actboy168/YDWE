#include "CC_Include.h"

int _fastcall 
CC_PutTrigger_Count(DWORD This, DWORD cc_gui_type)
{
  DWORD nItemCount, i;
  DWORD nItemClass;
  DWORD sum = 0;

  nItemCount = *(DWORD*)(This+0xC);
  
  for (i = 0; i < nItemCount; i++)
  {
    nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
    
    if (Proc_GetGUIType(nItemClass) == cc_gui_type)
    {
      if (*(DWORD*)(nItemClass+0x13C) != 0)
      {        
        sum++;
      }
    }
  }

  return sum;
}

void _fastcall 
CC_PutTrigger_PutExternProc(DWORD This, DWORD OutClass, char* name, DWORD cc_gui_type)
{
  DWORD nItemCount, i;
  DWORD nItemClass;

  nItemCount = *(DWORD*)(This+0xC);
  
  for (i = 0; i < nItemCount; i++)
  {
    nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
    
    if (Proc_GetGUIType(nItemClass) == cc_gui_type)
    {
      if (*(DWORD*)(nItemClass+0x13C) != 0)
      {
		char NewName[260];
		BLZSStrPrintf(NewName, 260, "%sFunc%03d", name, i + 1);
		CC_PutExternFuncEx(This, OutClass, NewName);
      }
    }
  }
}

void _fastcall 
CC_PutTrigger_ECA_ExternProc(DWORD This, DWORD OutClass)
{
  char buff[260];
  char name[260];
  
  GetTriggerName_Trig(This, name, 260);

  // Event
  CC_PutTrigger_PutExternProc(This, OutClass, name, CC_GUI_TYPE_EVENT);
  
  // Condition
  if (0 != CC_PutTrigger_Count(This, CC_GUI_TYPE_CONDITION))
  {
    CC_PutTrigger_PutExternProc(This, OutClass, name, CC_GUI_TYPE_CONDITION);

    BLZSStrPrintf(buff, 260, "function %sConditions takes nothing returns boolean", name);
    PUT_CONST(buff, 1);

    CC_PutBegin();
    PUT_CONST("return (", 0);
    CC_PutTrigger_Condition_And(This, OutClass, name);
    PUT_CONST(")", 1);
	CC_PutEnd();
      
    PUT_CONST("endfunction", 1);
    PUT_CONST("", 1);
  }
  // Action
  CC_PutTrigger_PutExternProc(This, OutClass, name, CC_GUI_TYPE_ACTION);

  BLZSStrPrintf(buff, 260, "function %sActions takes nothing returns nothing", name);
  PUT_CONST(buff, 1);

  CC_PutLocal_Begin(This, OutClass, FALSE, TRUE);

  {
    DWORD nItemCount, i;
    DWORD nItemClass;

    nItemCount = *(DWORD*)(This+0xC);

    for (i = 0; i < nItemCount; i++)
    {
      nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
      if (Proc_GetGUIType(nItemClass) == CC_GUI_TYPE_ACTION)
      {
        if (*(DWORD*)(nItemClass+0x13C) != 0)
        {
          CC_PutAction(nItemClass, OutClass, name, i, 0);
        }
      }
    }
  }
    
  CC_PutLocal_End(This, OutClass, FALSE, TRUE);
  PUT_CONST("endfunction", 1);
  PUT_CONST("", 1);
}

void _fastcall CC_PutEventRegisterEx(DWORD This, DWORD OutClass, const char* triggername, const char* name, DWORD index);

void _fastcall CC_PutTrigger_ECA(DWORD This, DWORD OutClass, char* triggername)
{
  DWORD nItemCount, i;
  DWORD nItemClass;
  char buff[260];
  char name[260];
     
  GetTriggerName_Trig(This, name, 260);
  
  // Event
  nItemCount = *(DWORD*)(This+0xC);

  for (i = 0; i < nItemCount; i++)
  {
    nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
    if (Proc_GetGUIType(nItemClass) == CC_GUI_TYPE_EVENT)
    {
		CC_PutEventRegisterEx(nItemClass, OutClass, triggername, name, i);
    }
  }
	
  // Condition
  if (0 != CC_PutTrigger_Count(This, CC_GUI_TYPE_CONDITION))
  {
    CC_PutBegin();
    BLZSStrPrintf(buff, 260, "call TriggerAddCondition(%s, Condition(function %sConditions))", triggername, name);
    PUT_CONST(buff, 1);
    CC_PutEnd();
  }
  // Action
  CC_PutBegin();
  BLZSStrPrintf(buff, 260, "call TriggerAddAction(%s, function %sActions)", triggername, name);
  PUT_CONST(buff, 1);

  CC_PutEnd();
}

BOOL _fastcall CC_PutTrigger_HasDisableRegister(DWORD This)
{
	DWORD nItemCount = *(DWORD*)(This+0xC);
	DWORD nItemClass;
	DWORD i;

	for (i = 0; i < nItemCount; i++)
	{
		nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
		if (Proc_GetGUIType(nItemClass) == CC_GUI_TYPE_EVENT)
		{
			if (*(DWORD*)(nItemClass+0x13C) != 0)
			{
				if (0 == BLZSStrCmp((char*)(nItemClass+0x20), "YDWEDisableRegister", 0x7FFFFFFF))
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

static bool isIdentifier(const char* str)
{
	int len = BLZSStrLen(str);
	if (len <= 0 || str[len - 1] == '_' || isdigit(str[0])) {
		return false;
	}
	for (int i = 0; i < len; ++i) {
		if (str[i] != '_' && !isalnum(str[i])) {
			return false;
		}
	}
	return true;
}

static void _fastcall GetTriggerName_InitTrig(DWORD This, DWORD EDX, char* buf, int len)
{
	if (*(DWORD*)(This + 0x18)) {
		buf[0] = 0;
		return;
	}
	char szTemp[260];
	ConvertTriggerName((char*)This + 0x4C, szTemp, 260);
	BLZSStrPrintf(buf, len, "%s%s", "InitTrig_", szTemp);
	if (!isIdentifier(buf)) {
		char szTemp2[260];
		GetWEString("WESTRING_MELEEINITIALIZATION", szTemp2, 260, 0);
		if (0 == BLZSStrCmpI((char*)This + 0x4C, szTemp2, 0x7FFFFFFFu)) {
			BLZSStrPrintf(buf, len, "%s%s", "InitTrig_", "MELEE_INITIALIZATION");
		}
	}
}

static void _fastcall GetTriggerVar(DWORD This, DWORD EDX, char* buf, int len)
{
	if (*(DWORD*)(This + 0x18)) {
		buf[0] = 0;
		return;
	}
	char szTemp[260];
	ConvertTriggerName((char*)This + 0x4C, szTemp, 260);
	BLZSStrPrintf(buf, len, "%s%s%s", "gg_", "trg_", szTemp);
	if (!isIdentifier(buf)) {
		char szTemp2[260];
		GetWEString("WESTRING_MELEEINITIALIZATION", szTemp2, 260, 0);
		if (0 == BLZSStrCmpI((char*)This + 0x4C, szTemp2, 0x7FFFFFFFu)) {
			BLZSStrPrintf(buf, len, "%s%s%s", "gg_", "trg_", "MELEE_INITIALIZATION");
		}
	}
}

void _fastcall 
CC_PutTrigger_Hook(DWORD This, DWORD EDX, DWORD OutClass)
{
  char buff[260];
  char name[260];
  char InitTrig[260];
  BOOL HasDisableRegister = FALSE;

  SaveLoadCheck_Reset();

  *(DWORD*)(*(DWORD*)(This+0x48)+0x30) = This;

  CC_PutTrigger_ECA_ExternProc(This, OutClass);
  
  GetTriggerName_InitTrig(This, EDX, InitTrig, 260);
  PUT_CONST("//===========================================================================", 1);

  HasDisableRegister = CC_PutTrigger_HasDisableRegister(This);

  if (HasDisableRegister)
  {
	  BLZSStrPrintf(buff, 260, "function %s_Orig takes nothing returns nothing", InitTrig);
	  PUT_CONST(buff, 1);
  }
  else
  {
	  BLZSStrPrintf(buff, 260, "function %s takes nothing returns nothing", InitTrig);
	  PUT_CONST(buff, 1);
  }

  CC_PutBegin();
  GetTriggerVar(This, EDX, name, 260);
  BLZSStrPrintf(buff, 260, "set %s = CreateTrigger()", name);
  PUT_CONST(buff, 1);
  if (*(DWORD*)(This+0x24))
  {
    BLZSStrPrintf(buff, 260, "call DisableTrigger(%s)", name);
    PUT_CONST(buff, 1);
  }

  CC_PutEnd();
  PUT_CONST("#ifdef DEBUG", 1);
  CC_PutBegin();
  BLZSStrPrintf(buff, 260, "call YDWESaveTriggerName(%s, \"%s\")", name, (char*)(This+0x4C));
  PUT_CONST(buff, 1);
  CC_PutEnd();
  PUT_CONST("#endif", 1);


  CC_PutTrigger_ECA(This, OutClass, name);
  
  PUT_CONST("endfunction", 1);
  PUT_CONST("", 1);

  if (HasDisableRegister)
  {
	  BLZSStrPrintf(buff, 260, "function %s takes nothing returns nothing", InitTrig);
	  PUT_CONST(buff, 1);
	  PUT_CONST("endfunction", 1);
	  PUT_CONST("", 1);
  }

  *(DWORD*)(*(DWORD*)(This+0x48)+0x30) = 0;
}
