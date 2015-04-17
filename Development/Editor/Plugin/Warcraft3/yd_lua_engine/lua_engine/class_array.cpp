#include "class_array.h"
#include "lua_helper.h"
#include "jassbind.h"
#include "common.h"
#include <base/warcraft3/jass/global_variable.h>

namespace base { namespace warcraft3 { namespace lua_engine {

#define LUA_JASS_ARRAY "jarray_t"

	void jass_get_global_variable(lua::state* ls, jass::OPCODE_VARIABLE_TYPE opt, uint32_t value);

	jass::global_variable jarray_value(lua::state* ls)
	{
		ls->pushvalue(1);
		ls->pushstring("__value");
		ls->rawget(-2);
		jass::global_variable gv((hashtable::variable_node*)ls->tointeger(-1));
		ls->pop(2);
		return std::move(gv);
	}

	int jarray_index(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;
		jass::global_variable gv = jarray_value(ls);
		int32_t index = ls->checkinteger(2);
	
		if (!gv.array_vaild(index))
		{
			ls->pushnil();
			return 1;
		}
	
		jass_get_global_variable(ls, jass::opcode_type_remove_array(gv.type()), gv[index]);
		return 1;
	}
	
	int jarray_newindex(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;
		jass::global_variable gv = jarray_value(ls);
		int32_t index = ls->checkinteger(2);
	
		if (!gv.array_vaild(index))
		{
			return 0;
		}
	
		gv[index] = jass_read(ls, jass::opcode_type_to_var_type(gv.type()), 3);
		return 0;
	}
	
	int jarray_len(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;
		jass::global_variable gv((hashtable::variable_node*)ls->tointeger(lua_upvalueindex(1)));
		ls->pushinteger(gv.array_size());
		return 1;
	}

	int jarray_create(lua_State* L, uintptr_t value)
	{
		lua_newtable(L);
		luaL_getmetatable(L, LUA_JASS_ARRAY);
		lua_setmetatable(L, -2);
		lua_pushstring(L, "__value");
		lua_pushunsigned(L, value);
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
