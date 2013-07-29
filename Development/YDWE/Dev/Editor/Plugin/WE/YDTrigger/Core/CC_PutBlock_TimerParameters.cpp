#include "CC_Include.h"

extern BOOL g_local_in_mainproc;
extern BOOL g_bDisableSaveLoadSystem;
extern int  g_mother_id;
char* g_handle_string = NULL;

void _fastcall 
	CC_Put_GetTimerParameters(DWORD This, DWORD OutClass, char* type_name, char* handle_string)
{
	char buff[260];

	g_bDisableSaveLoadSystem = FALSE;

	BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(%s), 0x%08X)", type_name, handle_string, SStrHash((char*)&GetGUIVar_Value(This, 0)));
	PUT_CONST(buff, 0);
}

void _fastcall
	CC_Put_SetTimerParameters(DWORD This, DWORD OutClass, char* name, char* handle_string)
{
	char buff[260];
	int var_type = GetVarType(This, 0);

	if ((CC_TYPE__begin < var_type) && (var_type < CC_TYPE__end))
	{
		g_bDisableSaveLoadSystem = FALSE;

		BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(%s), 0x%08X, ", TypeName[var_type], handle_string, SStrHash((char*)&GetGUIVar_Value(This, 1)));
		PUT_CONST(buff, 0);
		PUT_VAR(This, 2);
		PUT_CONST(")", 1);
	}
}

void _fastcall
	CC_PutBlock_TimerParameters(DWORD This, DWORD OutClass, char* name, DWORD index, char* handle_string)
{
	DWORD nItemCount, i;
	DWORD nItemClass;
	char NewName[260];
	nItemCount = *(DWORD*)(This+0xC);

	for (i = 0; i < nItemCount; i++)
	{
		nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
		if (*(DWORD*)(nItemClass+0x13C) != 0)
		{
			if ((index) == -1 || (*(DWORD*)(nItemClass+0x154) == index))
			{  
				switch (*(DWORD*)(nItemClass+0x138))
				{
				case CC_GUIID_YDWESetAnyTypeLocalVariable:
					{
						BLZSStrPrintf(NewName, 260, "%sFunc%03d", name, i+1);
						CC_PutBegin();
						CC_Put_SetTimerParameters(nItemClass, OutClass, NewName, handle_string);
						CC_PutEnd();
						break;  
					}
				case CC_GUIID_YDWETimerStartMultiple:
				case CC_GUIID_YDWERegisterTriggerMultiple:
					ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_ILLEGAL_PARAMETER");
				default:
					{
						int old_mother_id = g_mother_id;
						g_mother_id = (0x8000 | (int)CC_GUIID_YDWETimerStartMultiple);
						g_handle_string = handle_string;
						CC_PutAction(nItemClass, OutClass, name, i, 0);
						g_mother_id = old_mother_id;
						g_handle_string = NULL;
					}
					break;
				}
			}
		}
	}
}
