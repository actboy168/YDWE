#include "jassbind.h"
#include "class_array.h"
#include "libs_runtime.h"
#include "common.h"
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/jass/func_value.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/global_variable.h>
#include <bee/lua/range.h>

namespace bee::lua {
	template <>
	int convert_to_lua(lua_State* L, const std::string& v)
	{
		lua_pushlstring(L, v.data(), v.size());
		return 1;
	}

	template <>
	int convert_to_lua(lua_State* L, const std::string_view& v)
	{
		lua_pushlstring(L, v.data(), v.size());
		return 1;
	}
}

namespace base { namespace warcraft3 { namespace lua_engine { 

	namespace globals {
		void jass_get_global_variable(lua_State* L, jass::OPCODE_VARIABLE_TYPE opt, uint32_t value);
	}

	namespace common {

	int jass_get(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);

		jass::func_value const* nf = jass::jass_func(name);
		if (nf && nf->is_valid())
		{
			lua_pushinteger(L, (uint32_t)(uintptr_t)nf);
			lua_pushcclosure(L, jass_call_closure, 1);
			return 1;
		}

		if (!is_gaming())
		{
			lua_pushnil(L);
			return 1;
		}

		jass::global_variable gv(name);
		if (gv.is_vaild())
		{
			if (!gv.is_array())
			{
				globals::jass_get_global_variable(L, gv.type(), gv);
				return 1;
			}
			else
			{
				switch (gv.type())
				{
				case jass::OPCODE_VARIABLE_INTEGER_ARRAY:
				case jass::OPCODE_VARIABLE_REAL_ARRAY:
				case jass::OPCODE_VARIABLE_STRING_ARRAY:
				case jass::OPCODE_VARIABLE_HANDLE_ARRAY:
				case jass::OPCODE_VARIABLE_BOOLEAN_ARRAY:
					return jarray_create(L, (uintptr_t)gv.ptr());
				default:
					lua_pushnil(L);
					return 1;
				}
			}
		}

		lua_pushnil(L);
		return 1;
	}

	int jass_set(lua_State* L)
	{
		if (!is_gaming())
		{
			return 0;
		}

		const char* name = luaL_checkstring(L, 2);
		jass::global_variable gv(name);
		if (gv.is_vaild())
		{
			if (!gv.is_array())
			{
				gv = jass_read(L, jass::opcode_type_to_var_type(gv.type()), 3);
			}
			else
			{
				switch (gv.type())
				{
				case jass::OPCODE_VARIABLE_INTEGER_ARRAY:
				case jass::OPCODE_VARIABLE_REAL_ARRAY:
				case jass::OPCODE_VARIABLE_STRING_ARRAY:
				case jass::OPCODE_VARIABLE_HANDLE_ARRAY:
				case jass::OPCODE_VARIABLE_BOOLEAN_ARRAY:
					break;
				default:
					break;
				}
			}
		}

		return 0;
	}

	int jass_pairs(lua_State* L)
	{
		return bee::lua::make_range(L, jass::jass_function);
	}

	static void init_sleep_function(const char* name)
	{
		auto it = jass::jass_function.find(name);
		if (it == jass::jass_function.end() || !it->second.is_valid())
		{
			return;
		}
		it->second.set_sleep(true);
	}

	int open(lua_State* L)
	{
		init_sleep_function("TriggerSleepAction");
		init_sleep_function("TriggerWaitForSound");
		init_sleep_function("TriggerSyncReady");
		init_sleep_function("SyncSelections");

		lua_newtable(L);
		{
			lua_newtable(L);
			{
				lua_pushstring(L, "__index");
				lua_pushcclosure(L, jass_get, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__newindex");
				lua_pushcclosure(L, jass_set, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__pairs");
				lua_pushcclosure(L, jass_pairs, 0);
				lua_rawset(L, -3);
			}
			lua_setmetatable(L, -2);
		}
		return 1;
	}
}}}}
