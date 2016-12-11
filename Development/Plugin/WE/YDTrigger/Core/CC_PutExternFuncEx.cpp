#include "CC_Include.h"
#include "locvar.h"

void _fastcall CC_PutExternFuncEx(DWORD This, DWORD OutClass, char* name);
void _fastcall CC_PutActionEx_Hook(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD Type, DWORD Endl);

void _fastcall CC_PutExternFunc(DWORD This, DWORD OutClass, char* name, DWORD index)
{
	char NewName[260];
	BLZSStrPrintf(NewName, 260, "%sFunc%03d", name, index+1);

	CC_PutExternFuncEx(This, OutClass, NewName);
}

void _fastcall CC_PutExternFuncEx_Item(DWORD This, DWORD OutClass, char* name)
{
	DWORD nItemCount, i;
	DWORD nItemClass;

	nItemCount = *(DWORD*)(This+0x0C);

	for (i = 0; i < nItemCount; i++)
	{
		nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];

		if (*(DWORD*)(nItemClass+0x13C) != 0)
		{
			CC_PutExternFunc(nItemClass, OutClass, name, i);
		}
	}
}

void _fastcall CC_PutExternFuncEx_TopDown(DWORD This, DWORD OutClass, char* name)
{
	char NewName[260];
	char buff[260];
	DWORD nVar, VarClass, Var, i;
	LONG flag = -1;

	nVar = *(DWORD*)(This+0x128);

	for (i = 0; i < nVar; i++)
	{
		Var = ((DWORD*)(*(DWORD*)((This)+0x12C)))[i];
		VarClass = GetGUIVar_Class(This, i);

		switch (GetGUIVar_Type(This, i))
		{
		case CC_VARTYPE_boolexpr:
		case CC_VARTYPE_boolcall:
			flag = 1;
			break;
		case CC_VARTYPE_code:
			flag = 2;
			break;
		case CC_VARTYPE_eventcall:
			flag = 0;
			break;
		default:
			flag = -1;
			break;
		}

		if ((2 == (*(DWORD*)(Var+0x08))) && (0 == BLZSStrCmp((char*)(Var+0x0C), "boolexpr", 0xFFFFFFFF)))
		{
			flag = 1;
		}
		else if (flag == -1 && VarClass == 0)
		{
			continue;
		}

		if (0 == BLZSStrCmp((char*)(VarClass+0x20), "Null", 0xFFFFFFFF))
			continue;

		BLZSStrPrintf(NewName, 260, "%s%03d", name, i+1);
		CC_PutExternFuncEx(VarClass, OutClass, NewName);

		if (*(DWORD*)(VarClass+0x138) == CC_GUIID_ReturnAction)
			continue;

		switch (*(DWORD*)(This+0x138))
		{   
		case CC_GUIID_IfThenElse:
		case CC_GUIID_GetBooleanAnd:
		case CC_GUIID_GetBooleanOr:
			flag = -1;
			break;
		}

		if (flag == 1)
		{
			BLZSStrPrintf(buff, 260, "function %s takes nothing returns boolean", NewName);
			PUT_CONST(buff, 1);

			CC_PutBegin();
			PUT_CONST("return (", 0); 
			CC_PutConditionEx(VarClass, OutClass, NewName);
			PUT_CONST(")", 1); 
			CC_PutEnd();

			PUT_CONST("endfunction", 1);
			PUT_CONST("", 1);
		}
		else if ((flag == 2) && (!((CC_GUIID_IfThenElse <= *(DWORD*)(This+0x138)) && (*(DWORD*)(This+0x138) <= CC_GUIID_ForLoopVarMultiple))))
		{
			BLZSStrPrintf(buff, 260, "function %s takes nothing returns nothing", NewName);
			PUT_CONST(buff, 1);

			CC_PutActionEx_Hook(VarClass, 0, OutClass, NewName, CC_GUI_TYPE_ACTION, 0);

			PUT_CONST("endfunction", 1);
			PUT_CONST("", 1);
		}
	}

	CC_PutExternFuncEx_Item(This, OutClass, name);

	switch (*(DWORD*)(This+0x138))
	{
	case CC_GUIID_ForGroupMultiple:
	case CC_GUIID_ForForceMultiple:
	case CC_GUIID_EnumDestructablesInRectAllMultiple:
	case CC_GUIID_EnumDestructablesInCircleBJMultiple:
	case CC_GUIID_EnumItemsInRectBJMultiple:
		{
			BLZSStrPrintf(buff, 260, "function %sA takes nothing returns nothing", name);
			PUT_CONST(buff, 1);

			CC_PutLocal_Begin(This, OutClass, FALSE, FALSE);

			CC_PutBlock_Action(This, OutClass, name, -1);

			CC_PutLocal_End(This, OutClass, FALSE, TRUE);

			PUT_CONST("endfunction", 1);
			PUT_CONST("", 1);
		}
		break;
	default:
		break;
	}
}

void _fastcall CC_PutExternFuncEx(DWORD This, DWORD OutClass, char* name)
{
	switch (*(DWORD*)(This+0x138))
	{
	case CC_GUIID_YDWETimerStartMultiple:
		{
			locvar::guard _tmp_guard_(CC_GUIID_YDWETimerStartMultiple, name, "GetExpiredTimer()");

			CC_PutExternFuncEx_TopDown(This, OutClass, name);

			char buff[260];
			BLZSStrPrintf(buff, 260, "function %sT takes nothing returns nothing", name);
			PUT_CONST(buff, 1);

			CC_PutLocal_Begin(This, OutClass, TRUE, FALSE);
			CC_PutBlock_Action(This, OutClass, name, 1);
			CC_PutLocal_End(This, OutClass, TRUE, TRUE);

			PUT_CONST("endfunction", 1);
			PUT_CONST("", 1);
		}
		break;
	case CC_GUIID_YDWERegisterTriggerMultiple:
		{
			locvar::guard _tmp_guard_(CC_GUIID_YDWERegisterTriggerMultiple, name, "GetTriggeringTrigger()");

			CC_PutExternFuncEx_TopDown(This, OutClass, name);

			char buff[260];
			BLZSStrPrintf(buff, 260, "function %sConditions takes nothing returns nothing", name);
			PUT_CONST(buff, 1);

			CC_PutLocal_Begin(This, OutClass, TRUE, FALSE);
			CC_PutBlock_Action(This, OutClass, name, 2);
			CC_PutLocal_End(This, OutClass, TRUE, TRUE);

			PUT_CONST("endfunction", 1);
			PUT_CONST("", 1);
		}
		break;
	default:
		CC_PutExternFuncEx_TopDown(This, OutClass, name);
		break;
	}
}

