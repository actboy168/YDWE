#include "CC_Include.h"
#include "locvar.h"

extern int g_mother_id;
extern BOOL g_bDisableSaveLoadSystem;
extern BOOL g_local_in_mainproc;
extern char* g_handle_string;

int _fastcall Utf8toAscii(char src[], char dst[], unsigned int limit);

#define YDL_LOCALVAR_STEPS "ydl_localvar_step"

namespace locvar
{
	void get(DWORD This, DWORD OutClass, char* type_name)
	{
		g_bDisableSaveLoadSystem = FALSE;

		char buff[260];

		if (g_mother_id == CC_GUIID_YDWETimerStartMultiple)
		{
			BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(%s), 0x%08X)", type_name, "GetExpiredTimer()", SStrHash((char*)&GetGUIVar_Value(This, 0)));
		}
		else if (g_mother_id == CC_GUIID_YDWERegisterTriggerMultiple)
		{
			BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(%s), 0x%08X)", type_name, "GetTriggeringTrigger()", SStrHash((char*)&GetGUIVar_Value(This, 0)));
		}
		else
		{
			LPCSTR lpszKey = (LPCSTR)&GetGUIVar_Value(This, 0);
			if (!SaveLoadCheck_Set(lpszKey, (LPCSTR)type_name)) 
			{
				char tmp[260];
				Utf8toAscii((char*)lpszKey, tmp, 260);
				ShowErrorN(OutClass, "WESTRING_ERROR_YDTRIGGER_LOCVAR", tmp, (LPCSTR)type_name, tmp, SaveLoadCheck_Get(lpszKey));
			}

			if (g_local_in_mainproc)
			{
				BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*"YDL_LOCALVAR_STEPS", 0x%08X)", type_name, SStrHash(lpszKey));
			}
			else
			{
				BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*YDTriggerGetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X), 0x%08X)", type_name, SStrHash("TriggerRunSteps"), SStrHash(lpszKey));
			}
		}

		PUT_CONST(buff, 0);
	}

	void set(DWORD This, DWORD OutClass, char* name)
	{
		CC_PutBegin();

		char buff[260];
		int var_type = GetVarType(This, 0);

		if ((CC_TYPE__begin < var_type) && (var_type < CC_TYPE__end))
		{
			g_bDisableSaveLoadSystem = FALSE;

			if (g_mother_id == CC_GUIID_YDWETimerStartMultiple)
			{
				BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(%s), 0x%08X, ", TypeName[var_type], "GetExpiredTimer()", SStrHash((char*)&GetGUIVar_Value(This, 1)));
			}
			else if (g_mother_id == (0x8000 | (int)CC_GUIID_YDWETimerStartMultiple))
			{
				BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(%s), 0x%08X, ", TypeName[var_type], g_handle_string, SStrHash((char*)&GetGUIVar_Value(This, 1)));
			}
			else if (g_mother_id == CC_GUIID_YDWERegisterTriggerMultiple)
			{
				BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(%s), 0x%08X, ", TypeName[var_type], "GetTriggeringTrigger()", SStrHash((char*)&GetGUIVar_Value(This, 1)));
			}
			else
			{
				LPCSTR lpszKey = (LPCSTR)&GetGUIVar_Value(This, 1);
				if (!SaveLoadCheck_Set(lpszKey, TypeName[var_type]))
				{
					char tmp[260];
					Utf8toAscii((char*)lpszKey, tmp, 260);
					ShowErrorN(OutClass, "WESTRING_ERROR_YDTRIGGER_LOCVAR", tmp, TypeName[var_type], tmp, SaveLoadCheck_Get(lpszKey));
				}

				if (g_local_in_mainproc)
				{
					BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*"YDL_LOCALVAR_STEPS", 0x%08X, ", TypeName[var_type], SStrHash(lpszKey));
				}
				else
				{
					BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*YDTriggerGetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X), 0x%08X, ", TypeName[var_type], SStrHash("TriggerRunSteps"), SStrHash(lpszKey));
				}
			}

			PUT_CONST(buff, 0);
			PUT_VAR(This, 2);
			PUT_CONST(")", 1);
		}
		CC_PutEnd();
	}
}
