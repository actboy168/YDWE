#include "class_array.h"
#include "lua_helper.h"
#include "jassbind.h"
#include "common.h"
#include <base/warcraft3/jass/global_variable.h>

namespace base { namespace warcraft3 { namespace lua_engine {
	namespace globals {
		void jass_get_global_variable(lua_State* L, jass::OPCODE_VARIABLE_TYPE opt, uint32_t value);
	}

#define LUA_JASS_ARRAY "jarray_t"

	jass::global_variable jarray_value(lua_State* L)
	{
		lua_pushvalue(L, 1);
		lua_pushstring(L, "__value");
		lua_rawget(L, -2);
		jass::global_variable gv((hashtable::variable_node*)lua_tointeger(L, -1));
		lua_pop(L, 2);
		return std::move(gv);
	}

	int jarray_index(lua_State* L)
	{
		jass::global_variable gv = jarray_value(L);
		int32_t index = (int32_t)luaL_checkinteger(L, 2);

		if (!gv.array_vaild(index))
		{
			lua_pushnil(L);
			return 1;
		}
	
		globals::jass_get_global_variable(L, jass::opcode_type_remove_array(gv.type()), gv[index]);
		return 1;
	}
	
	int jarray_newindex(lua_State* L)
	{
		jass::global_variable gv = jarray_value(L);
		int32_t index = (int32_t)luaL_checkinteger(L, 2);
	
		if (!gv.array_vaild(index))
		{
			return 0;
		}
	
		gv[index] = jass_read(L, jass::opcode_type_to_var_type(gv.type()), 3);
		return 0;
	}
	
	int jarray_len(lua_State* L)
	{
		jass::global_variable gv((hashtable::variable_node*)lua_tointeger(L, lua_upvalueindex(1)));
		lua_pushinteger(L, gv.array_size());
		return 1;
	}

	int jarray_create(lua_State* L, uintptr_t value)
	{
		lua_newtable(L);
		luaL_getmetatable(L, LUA_JASS_ARRAY);
		lua_setmetatable(L, -2);
		lua_pushstring(L, "__value");
		lua_pushinteger(L, value);
		lua_rawset(L, -3);
		return 1;
	}

	void jarray_make_mt(lua_State* L)
	{
		luaL_Reg lib[] = {
			{ "__index",    jarray_index },
			{ "__newindex", jarray_newindex },
			{ "__len",      jarray_len },
			{ NULL, NULL },
		};
	
		luaL_newmetatable(L, LUA_JASS_ARRAY);
#if LUA_VERSION_NUM >= 502
		luaL_setfuncs(L, lib, 0);
		lua_pop(L, 1);
#else
		luaL_register(L, NULL, lib);
#endif
	}
}}}
