#include "CC_Include.h"
#include "locvar.h"
#include <map>
#include <set>
#include <string>
#include <functional>

extern BOOL g_bDisableSaveLoadSystem;
extern BOOL g_local_in_mainproc;

int _fastcall Utf8toAscii(char src[], char dst[], unsigned int limit);

#define YDL_LOCALVAR_STEPS "ydl_localvar_step"

namespace locvar
{
	extern std::map<std::string, std::string> trigger_data_ui;

	std::map<std::string, std::map<std::string, std::string>> register_var;

	state global;

	guard::guard(int id, const char* name, const char* handle_string)
		: old_(global)
	{
		current().last_mother_id     = global.mother_id;
		current().prev_handle_string = current().handle_string;
		current().mother_id          = id;
		current().name               = name;
		current().handle_string      = handle_string;
	}

	guard::~guard()
	{
		global = old_;
	}

	state& guard::current()
	{
		return global;
	}

	void do_get(DWORD OutClass, const char* type_name, const char* var_name, const state& s)
	{
		g_bDisableSaveLoadSystem = FALSE;

		char buff[260];

		if ((s.mother_id == (0x10000 | (int)CC_GUIID_YDWETimerStartMultiple)) && (s.prev_handle_string != nullptr))
		{
			register_var[s.name][var_name] = type_name;
			BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(%s), 0x%08X)", type_name, s.prev_handle_string, SStrHash(var_name));
		}
		else if ((s.mother_id == CC_GUIID_YDWETimerStartMultiple) || (s.mother_id == CC_GUIID_YDWERegisterTriggerMultiple))
		{
			register_var[s.name][var_name] = type_name;
			BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(%s), 0x%08X)", type_name, s.handle_string, SStrHash(var_name));
		}
		else
		{
			if (!SaveLoadCheck_Set(var_name, (LPCSTR)type_name)) 
			{
				char tmp[260];
				Utf8toAscii((char*)var_name, tmp, 260);
				ShowErrorN(OutClass, "WESTRING_ERROR_YDTRIGGER_LOCVAR", tmp, (LPCSTR)type_name, tmp, SaveLoadCheck_Get(var_name));
			}

			if (g_local_in_mainproc)
			{
				BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*"YDL_LOCALVAR_STEPS", 0x%08X)", type_name, SStrHash(var_name));
			}
			else
			{
				BLZSStrPrintf(buff, 260, "YDTriggerGetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*YDTriggerGetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X), 0x%08X)", type_name, SStrHash("TriggerRunSteps"), SStrHash(var_name));
			}
		}

		PUT_CONST(buff, 0);
	}

	void do_set(DWORD OutClass, const char* type_name, const char* var_name, const state& s, std::function<void(void)> func)
	{
		g_bDisableSaveLoadSystem = FALSE;
		char buff[260];

		CC_PutBegin();

		if ((s.mother_id == (0x10000 | CC_GUIID_YDWETimerStartMultiple))
			|| (s.mother_id == CC_GUIID_YDWETimerStartMultiple)
			|| (s.mother_id == CC_GUIID_YDWERegisterTriggerMultiple))
		{
			BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(%s), 0x%08X, ", type_name, s.handle_string, SStrHash(var_name));
		}
		else
		{
			if (!SaveLoadCheck_Set(var_name, type_name))
			{
				char tmp[260];
				Utf8toAscii((char*)var_name, tmp, 260);
				ShowErrorN(OutClass, "WESTRING_ERROR_YDTRIGGER_LOCVAR", tmp, type_name, tmp, SaveLoadCheck_Get(var_name));
			}

			if (g_local_in_mainproc)
			{
				BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*" YDL_LOCALVAR_STEPS ", 0x%08X, ", type_name, SStrHash(var_name));
			}
			else
			{
				BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(%s, YDTriggerH2I(GetTriggeringTrigger())*YDTriggerGetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X), 0x%08X, ", type_name, SStrHash("TriggerRunSteps"), SStrHash(var_name));
			}
		}

		PUT_CONST(buff, 0);
		func();
		PUT_CONST(")", 1);

		CC_PutEnd();

		if (s.mother_id == (0x10000 | CC_GUIID_YDWETimerStartMultiple))
		{
			register_var[s.name].erase(var_name);
		}
	}

	void get(DWORD This, DWORD OutClass, char* type_name)
	{
		do_get(OutClass, type_name, (LPCSTR)&GetGUIVar_Value(This, 0), global);
	}

	void set(DWORD This, DWORD OutClass, char* name)
	{
		int var_type = GetVarType(This, 0);

		if ((CC_TYPE__begin < var_type) && (var_type < CC_TYPE__end))
		{
			do_set(OutClass, TypeName[var_type], (LPCSTR)&GetGUIVar_Value(This, 1), global, [&](){ PUT_VAR(This, 2); } );
		}
	}

	void flush_in_timer(DWORD This, DWORD OutClass)
	{
		if (global.mother_id == CC_GUIID_YDWETimerStartMultiple)
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
		if (global.mother_id == CC_GUIID_YDWERegisterTriggerMultiple)
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
		if (global.mother_id != CC_GUIID_YDWETimerStartMultiple
			&& global.mother_id != CC_GUIID_YDWERegisterTriggerMultiple)
		{
			char buff[260];

			if (g_local_in_mainproc)
			{
				g_bDisableSaveLoadSystem = FALSE;
				BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X, " YDL_LOCALVAR_STEPS ")", SStrHash("TriggerRunSteps"));
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
		if (global.mother_id == CC_GUIID_YDWETimerStartMultiple
			|| global.mother_id == CC_GUIID_YDWERegisterTriggerMultiple)
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
			BLZSStrPrintf(buff, 260, "local integer " YDL_LOCALVAR_STEPS " = YDTriggerGetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X)", SStrHash("GlobalsTriggerRunSteps"));
			PUT_CONST(buff, 1);
			PUT_CONST("set " YDL_LOCALVAR_STEPS " = " YDL_LOCALVAR_STEPS " + 3", 1);
			BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X, " YDL_LOCALVAR_STEPS ")", SStrHash("GlobalsTriggerRunSteps"));
			PUT_CONST(buff, 1);
			BLZSStrPrintf(buff, 260, "call YDTriggerSetEx(integer, YDTriggerH2I(GetTriggeringTrigger()), 0x%08X, " YDL_LOCALVAR_STEPS ")", SStrHash("TriggerRunSteps"));
			PUT_CONST(buff, 1);
			CC_PutEnd();
		}
	}

	void destroy(DWORD OutClass)
	{
		if (g_local_in_mainproc)
		{
			CC_PutBegin(); 
			PUT_CONST("call YDTriggerClearTable(YDTriggerH2I(GetTriggeringTrigger())*" YDL_LOCALVAR_STEPS ")", 1);
			CC_PutEnd();
		}
	}

	void params(DWORD This, DWORD OutClass, char* name, DWORD index, char* handle_string)
	{
		std::set<std::string> paramlist;

		{
			locvar::guard _tmp_guard_((0x10000 | (int)CC_GUIID_YDWETimerStartMultiple), name, handle_string);

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
								paramlist.insert((const char*)&GetGUIVar_Value(nItemClass, 1));
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
		}

		for each (auto it in register_var[name])
		{
			if (paramlist.find(it.first) != paramlist.end())
			{
				continue;
			}

			auto uiit = trigger_data_ui.find(it.first);
			if (uiit == trigger_data_ui.end())
			{
				locvar::do_set(OutClass, it.second.c_str(), it.first.c_str()
					, state(CC_GUIID_YDWETimerStartMultiple, name, handle_string)
					, [&]()
					{
						locvar::do_get(OutClass, it.second.c_str(), it.first.c_str(), global); 
					}
				);
			}
			else
			{
				locvar::do_set(OutClass, it.second.c_str(), it.first.c_str()
					, state(CC_GUIID_YDWETimerStartMultiple, name, handle_string)
					, [&]()
				{
					if (global.mother_id == CC_GUIID_YDWETimerStartMultiple)
					{
						locvar::do_get(OutClass, it.second.c_str(), it.first.c_str(), global);
						//PUT_CONST(it.first.c_str(), 0);
						//PUT_CONST("()", 0);
					}
					else
					{
						PUT_CONST(it.first.c_str(), 0);
						PUT_CONST("()", 0);
					}
				});
			}
		}
		register_var.erase(name);
	}

	bool trigger_data(DWORD This, DWORD OutClass, const char* name)
	{
		if (global.mother_id == (0x10000 | (int)CC_GUIID_YDWETimerStartMultiple))
		{
			if (global.last_mother_id != CC_GUIID_YDWETimerStartMultiple)
				return false;
		}
		else if ((global.mother_id != CC_GUIID_YDWETimerStartMultiple))
		{
			return false;
		}

		auto it = trigger_data_ui.find(name);
		if (it == trigger_data_ui.end())
		{
			return false;
		}
		
		locvar::do_get(OutClass, it->second.c_str(), it->first.c_str(), global);
		return true;
	}
}
