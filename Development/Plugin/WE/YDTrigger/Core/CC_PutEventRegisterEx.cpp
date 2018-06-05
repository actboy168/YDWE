#include "CC_Include.h"

BOOL _fastcall CC_PutAction_SearchVar(DWORD This, DWORD OutClass)
{
	DWORD nVarCount, i;
	DWORD nVarClass;

	nVarCount = *(DWORD*)(This+0x128);

	for (i = 0; i < nVarCount; i++)
	{
		if (0 == BLZSStrCmp((char*)&GetGUIVar_Value(This, i), "PlayerALL", 0xFFFFFFFF))
			return TRUE;

		nVarClass = GetGUIVar_Class(This, i);
		if (nVarClass != 0)
		{
			if (CC_PutAction_SearchVar(nVarClass, OutClass))
				return TRUE;
		}    
	}

	return FALSE;
}
int _fastcall CC_PutEventRegister(DWORD This, DWORD EDX, DWORD OutClass, const char* triggername, const char* name, DWORD index)
{
	if (0 == BLZSStrCmp((char *)This + 32, "MapInitializationEvent", -1)) {
		return 0;
	}

	char guiName[260];
	CC_GetGUIName(This, 0, guiName, 260);
	PUT_CONST("    call ", 0);
	PUT_CONST(guiName, 0);
	PUT_CONST("(", 0);
	PUT_CONST(triggername, 0);
	char NewName[260];
	BLZSStrPrintf(NewName, 260, "%sFunc%03d", name, index + 1);
	DWORD Type = (*(int(__thiscall **)(int))(*(DWORD *)This + 8))(This);
	DWORD nVar = *(DWORD*)(This + 0x128);
	for (DWORD i = 0; i < nVar; ++i) {
		PUT_CONST(", ", 0);
		CC_PutVar((This), 0, OutClass, (char*)name, i, Type, 0);
	}
	return PUT_CONST(")", 1);
}

void _fastcall CC_PutEventRegisterEx(DWORD This, DWORD OutClass, const char* triggername, const char* name, DWORD index)
{
	if (*(DWORD*)(This+0x13C) != 0)
	{
		BOOL bFlag = FALSE;
		if (CC_PutAction_SearchVar(This, OutClass))
		{
			PUT_CONST("#define YDTRIGGER_COMMON_LOOP(n) ", 0);
			
			bFlag = TRUE;
		}

		if (0 != BLZSStrCmp((char*)(This+0x20), "YDWEDisableRegister", 0x7FFFFFFF))
		{
			CC_PutEventRegister(This, 0, OutClass, triggername, name, index);
		}

		if (bFlag)
		{
			PUT_CONST("#define YDTRIGGER_COMMON_LOOP_LIMITS (0, 15)", 1);
			PUT_CONST("#include <YDTrigger/Common/loop.h>", 1);
		}
	}
}
