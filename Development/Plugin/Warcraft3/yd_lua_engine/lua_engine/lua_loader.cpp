#include "callback.h"
#include "fix_baselib.h"
#include "lua_loader.h"
#include "storm.h"
#include "open_lua_engine.h"
#include "libs_runtime.h"
#include <lua.hpp>
#include <warcraft3/event.h>
#include <warcraft3/jass.h>
#include <warcraft3/jass/func_value.h>
#include <warcraft3/jass/hook.h>
#include <base/util/singleton.h>
#include <bee/utility/format.h>
#include <base/hook/fp_call.h>
#include <string_view>
#include <base/util/string_algorithm.h>
#include <warcraft3/virtual_mpq.h>

namespace base { namespace warcraft3 { namespace lua_engine { namespace lua_loader {

	static char tmpMapName[1024] = { 0 };
	static char curMapName[1024] = { 0 };
	static bool init = false;
	static lua_State* mainL = 0;
	static lua_State* getMainL()
	{
		if (!mainL) {
			lua_State* L = newstate();
			if (L) {
				luaL_openlibs(L);
				open_lua_engine(L);
				runtime::initialize();
			}
			mainL = L;
		}
		return mainL;
	}

	uintptr_t RealCheat = 0;
	void __cdecl FakeCheat(jass::jstring_t cheat_str)
	{
		const char* cheat = jass::from_string(cheat_str);

		if (!cheat)
		{
			c_call<uint32_t>(RealCheat, cheat_str);
			return ;
		}

		std::string_view cheat_s = cheat;
		
		if (cheat_s.substr(0, 9) == "exec-lua:")
		{
			cheat_s = cheat_s.substr(9);
			algorithm::trim(cheat_s);
			if (cheat_s.size() >= 2 && cheat_s[0] == '"' && cheat_s[cheat_s.size() - 1] == '"')
			{
				cheat_s = cheat_s.substr(1, cheat_s.size() - 2);
			}
			lua_State* L = getMainL();
			lua_getglobal(L, "require");
			lua_pushlstring(L, cheat_s.data(), cheat_s.size());
			safe_call(L, 1, 1, true);
		}

		c_call<uint32_t>(RealCheat, cheat_str);
	}

	jass::jstring_t __cdecl EXExecuteScript(jass::jstring_t script)
	{
		lua_State* L = getMainL();

		std::string str_script = bee::format("return (%s)", jass::from_trigstring(jass::from_string(script)));
		if (luaL_loadbuffer(L, str_script.c_str(), str_script.size(), str_script.c_str()) != LUA_OK)
		{
			printf("%s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
			return 0;
		}

		if (LUA_OK != safe_call(L, 0, 1, true))
		{
			return 0;
		}

		jass::jstring_t result = 0;
		if (lua_isstring(L, -1))
		{
			result = jass::create_string(lua_tostring(L, -1));
		}
		lua_pop(L, 1);
		return result;
	}

	static void initialize_lua()
	{
		const char* buf = 0;
		size_t      len = 0;
		if (storm_s::instance().load_file("script\\war3map.lua", (const void**)&buf, &len))
		{
			lua_State* L = getMainL();
			if (luaL_loadbuffer(L, buf, len, "@script\\war3map.lua") != LUA_OK) {
				printf("%s\n", lua_tostring(L, -1));
				lua_pop(L, 1);
				storm_s::instance().unload_file(buf);
				return;
			}
			safe_call(L, 0, 0, true);
			storm_s::instance().unload_file(buf);
		}
		else
		{
			jass::table_hook("Cheat", (uintptr_t*)&RealCheat, (uintptr_t)FakeCheat);
			jass::japi_table_add((uintptr_t)EXExecuteScript, "EXExecuteScript", "(S)S");
		}
	}

	void initialize()
	{
		virtual_mpq::force_watch("war3map.j", [&](const std::string&, const void**, uint32_t*, uint32_t)->bool {
			if (!storm_s::instance().get_mpq_name(tmpMapName, sizeof tmpMapName - 1)) {
				return false;
			}
			if (curMapName[0] && strcmp(curMapName, tmpMapName) == 0) {
				return false;
			}
			strcpy(curMapName, tmpMapName);
			initialize_lua();
			return false;
		});

		event_game_reset([&]()
		{
			if (mainL)
			{
				lua_close(mainL);
				mainL = 0;
			}
			curMapName[0] = 0;
		});
	}
}}}}
