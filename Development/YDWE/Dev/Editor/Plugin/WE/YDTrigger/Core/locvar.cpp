#include "CC_Include.h"
#include "locvar.h"

extern BOOL g_bDisableSaveLoadSystem;
extern BOOL g_local_in_mainproc;

int _fastcall Utf8toAscii(char src[], char dst[], unsigned int limit);

#define YDL_LOCALVAR_STEPS "ydl_localvar_step"

namespace locvar
{
	namespace global
	{
		int         mother_id = 0;
		const char* handle_string = NULL;
	}

	guard::guard(int id)
		: old_(global::mother_id)
	{ 
		global::mother_id = id;
	}

	guard::~guard()
	{
		global::mother_id = old_;
		global::handle_string = NULL;
	}

	void guard::set_handle(const char* handle_name)
	{
		global::handle_string = handle_name;
	}

	void get(DWORD This, DWORD OutClass, char* type_name)
	{
		g_bDisableSaveLoadSystem = FALSE;

		char buff[260];

		LPCSTR lpszKey = (LPCSTR)&GetGUIVar_Value(This, 0);

		if (global::mother_id == CC_GUIID_YDWETimerStartMultiple)
		{
			BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(%s), 0x%08X)", type_name, "GetExpiredTimer()", SStrHash(lpszKey));
		}
		else if (global::mother_id == CC_GUIID_YDWERegisterTriggerMultiple)
		{
			BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(%s), 0x%08X)", type_name, "GetTriggeringTrigger()", SStrHash(lpszKey));
		}
		else
		{
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
		LPCSTR lpszKey = (LPCSTR)&GetGUIVar_Value(This, 1);

		if ((CC_TYPE__begin < var_type) && (var_type < CC_TYPE__end))
		{
			g_bDisableSaveLoadSystem = FALSE;

			if (global::mother_id == CC_GUIID_YDWETimerStartMultiple)
			{
				BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(%s), 0x%08X, ", TypeName[var_type], "GetExpiredTimer()", SStrHash(lpszKey));
			}
			else if (global::mother_id == (0x8000 | (int)CC_GUIID_YDWETimerStartMultiple))
			{
				BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(%s), 0x%08X, ", TypeName[var_type], global::handle_string, SStrHash(lpszKey));
			}
			else if (global::mother_id == CC_GUIID_YDWERegisterTriggerMultiple)
			{
				BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(%s), 0x%08X, ", TypeName[var_type], "GetTriggeringTrigger()", SStrHash(lpszKey));
			}
			else
			{
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

	void flush_in_timer(DWORD This, DWORD OutClass)
	{
		if (global::mother_id == CC_GUIID_YDWETimerStartMultiple)
		{
			g_bDisableSaveLoadSystem = FALSE;

			CC_PutBegin();
			PUT_CONST("call YDTriggerClearTable(YDTriggerH2I(GetExpiredTimer()))", 1);
			PUT_CONST("call DestroyTimer(GetExpiredTimer())", 1);
			CC_PutEnd();
		}
		else
		{
			ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_TIMER_FLUSH");
		}
	}

	void flush_in_trigger(DWORD This, DWORD OutClass)
	{
		if (global::mother_id == CC_GUIID_YDWERegisterTriggerMultiple)
		{
			g_bDisableSaveLoadSystem = FALSE;

			CC_PutBegin();
			PUT_CONST("call YDTriggerClearTable(YDTriggerH2I(GetTriggeringTrigger()))", 1);
			PUT_CONST("call DestroyTrigger(GetTriggeringTrigger())", 1);
			CC_PutEnd();
		}
		else
		{
			ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_TRIGGER_FLUSH");
		}
	}


	void sleep_after(DWORD This, DWORD OutClass)
	{
		if (global::mother_id != CC_GUIID_YDWETimerStartMultiple
			&& global::mother_id != CC_GUIID_YDWERegisterTriggerMultiple)
		{
			char buff[260];

			if (g_local_in_mainproc)
			{
				g_bDisableSaveLoadSystem = FALSE;
				BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X, "YDL_LOCALVAR_STEPS")", SStrHash("TriggerRunSteps"));
				PUT_CONST(buff, 1);
			}
		}
		else
		{
			ShowError(OutClass, "WESTRING_ERROR_YDTRIGGER_ILLEGAL_WAIT");
		}
	}

	void return_before(DWORD This, DWORD OutClass)
	{
		if (global::mother_id == CC_GUIID_YDWETimerStartMultiple
			|| global::mother_id == CC_GUIID_YDWERegisterTriggerMultiple)
		{
			CC_PutLocal_End(This, OutClass, TRUE, FALSE);
		}
		else
		{
			CC_PutLocal_End(This, OutClass, FALSE, FALSE);
		}
	}

	void construct(DWORD OutClass)
	{
		char buff[260];

		if (g_local_in_mainproc)
		{
			g_bDisableSaveLoadSystem = FALSE;

			CC_PutBegin();
			BLZSStrPrintf(buff, 260, "local integer "YDL_LOCALVAR_STEPS" = YDTriggerGetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X)", SStrHash("GlobalsTriggerRunSteps"));
			PUT_CONST(buff, 1);
			PUT_CONST("set "YDL_LOCALVAR_STEPS" = "YDL_LOCALVAR_STEPS" + 3", 1);
			BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X, "YDL_LOCALVAR_STEPS")", SStrHash("GlobalsTriggerRunSteps"));
			PUT_CONST(buff, 1);
			BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X, "YDL_LOCALVAR_STEPS")", SStrHash("TriggerRunSteps"));
			PUT_CONST(buff, 1);
			CC_PutEnd();
		}
	}

	void destroy(DWORD OutClass)
	{
		if (g_local_in_mainproc)
		{
			CC_PutBegin();
			PUT_CONST("call YDTriggerClearTable(YDTriggerH2I(GetTriggeringTrigger())*"YDL_LOCALVAR_STEPS")", 1);
			CC_PutEnd();
		}
	}
}
