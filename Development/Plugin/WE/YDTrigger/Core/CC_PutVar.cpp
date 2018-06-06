#include <Windows.h>
#include <BlizzardStorm.h>
#include "../Common.h"
#include "CC_VarType.h"

int  _fastcall CC_PutVar_Other_Hook(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD index, DWORD type);

int _fastcall CC_PutVar(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD Index, DWORD Type, DWORD Endl)
{
	char szName[260];
	char szTemp[260];
	LONG flag = -1;
	DWORD VarType = GetGUIVar_Type(This, Index);
	switch (VarType)
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
	default: {
		DWORD Var = ((DWORD*)(*(DWORD*)((This)+0x12C)))[Index];
		if ((2 != (*(DWORD*)(Var + 0x08))) || (0 != BLZSStrCmp((char*)(Var + 0x0C), "boolexpr", 0xFFFFFFFF)))
		{
			return CC_PutVar_Other_Hook(Var, 0, OutClass, name, Index, Type);
		}
		break;
	}
	}

	DWORD VarClass = GetGUIVar_Class(This, Index);
	if (!BLZSStrCmp((char*)(VarClass + 0x20), "Null", 0x7FFFFFFFu))
	{
		BLZSStrPrintf(szName, 260, "null");
		return CC_PutConst(OutClass, szName, 0);
	}
	BLZSStrPrintf(szTemp, 260, "%s%03d", name, Index + 1);
	if (Endl)
	{
		BLZSStrCopy(szName, szTemp, 0x7FFFFFFFu);
		return CC_PutConst(OutClass, szName, 0);
	}
	if (flag != 1)
	{
		if (flag == 2)
		{
			BLZSStrPrintf(szName, 260, "function %s", szTemp);
			return CC_PutConst(OutClass, szName, 0);
		}
		return CC_PutConst(OutClass, szName, 0);
	}
	if (VarType == CC_VARTYPE_boolcall)
	{
		BLZSStrPrintf(szName, 260, "%s()", szTemp);
		return CC_PutConst(OutClass, szName, 0);
	}
	else
	{
		BLZSStrPrintf(szName, 260, "Condition(function %s)", szTemp);
		return CC_PutConst(OutClass, szName, 0);
	}
}
