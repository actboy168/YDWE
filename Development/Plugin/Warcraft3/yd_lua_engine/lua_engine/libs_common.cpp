#include "jassbind.h"
#include "class_array.h"
#include "libs_runtime.h"
#include "common.h"
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/jass/func_value.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/global_variable.h>
#include <base/lua/make_range.h>

namespace base { namespace warcraft3 { namespace lua_engine { 

	namespace globals {
		void jass_get_global_variable(lua_State* L, jass::OPCODE_VARIABLE_TYPE opt, uint32_t value);
	}

	namespace common {

	int jass_call_null_function(lua_State* L)
	{
		printf("Wanring: %s isn't implemented.\n", lua_tostring(L, lua_upvalueindex(1)));
		lua_pushnil(L);
		return 1;
	}

	int jass_get(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);

		jass::func_value const* nf = jass::jass_func(name);
		if (nf && nf->is_valid())
		{
			if (!lua_isyieldable(L))
			{
				if ((0 == strcmp(name, "TriggerSleepAction"))
					|| (0 == strcmp(name, "TriggerWaitForSound"))
					|| (0 == strcmp(name, "TriggerSyncReady"))
					|| (0 == strcmp(name, "SyncSelections")))
				{
					lua_pushstring(L, name);
					lua_pushcclosure(L, jass_call_null_function, 1);
					return 1;
				}
			}

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

		const char* name = lua_tostring(L, 2);
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
		return lua::make_range(L, jass::jass_function);
	}

	int open(lua_State* L)
	{
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
