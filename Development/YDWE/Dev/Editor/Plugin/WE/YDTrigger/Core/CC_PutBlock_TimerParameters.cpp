#include "CC_Include.h"
#include "locvar.h"

extern BOOL g_local_in_mainproc;
extern BOOL g_bDisableSaveLoadSystem;
extern int  g_mother_id;
char* g_handle_string = NULL;

void _fastcall
	CC_PutBlock_TimerParameters(DWORD This, DWORD OutClass, char* name, DWORD index, char* handle_string)
{
	DWORD nItemCount, i;
	DWORD nItemClass;
	char NewName[260];
	nItemCount = *(DWORD*)(This+0xC);

	int old_mother_id = g_mother_id;
	g_mother_id = (0x8000 | (int)CC_GUIID_YDWETimerStartMultiple);
	g_handle_string = handle_string;

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
						locvar::set(nItemClass, OutClass, NewName);
						break;  
					}
				case CC_GUIID_YDWETimerStartMultiple:
				case CC_GUIID_YDWERegisterTriggerMultiple:
					ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_ILLEGAL_PARAMETER");
					break;
				default:
					{
						CC_PutAction(nItemClass, OutClass, name, i, 0);
					}
					break;
				}
			}
		}
	}

	g_mother_id = old_mother_id;
	g_handle_string = NULL;
}
