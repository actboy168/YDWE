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

void _fastcall CC_PutEventRegisterEx(DWORD This, DWORD OutClass, char* triggername, char* name, DWORD index)
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
