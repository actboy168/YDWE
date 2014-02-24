#include "class_array.h"
#include "../lua/helper.h"
#include "../lua/jassbind.h"
#include <base/warcraft3/jass/global_variable.h>

namespace base { namespace warcraft3 { namespace lua_engine {

#define LUA_JASS_ARRAY "jarray_t"

	uintptr_t jass_read(jassbind* lj, jass::variable_type opt, int idx);
	void jass_get_global_variable(jassbind* lj, jass::OPCODE_VARIABLE_TYPE opt, uint32_t value);

	jass::global_variable array_value(jassbind* lj)
	{
		lj->pushvalue(1);
		lj->pushstring("__value");
		lj->rawget(-2);
		jass::global_variable gv((hashtable::variable_node*)lj->checkunsigned(-1));
		lj->pop(2);
		return std::move(gv);
	}

	int array_index(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;
		jass::global_variable gv = array_value(lj);
		int32_t index = lj->checkinteger(2);
	
		if (!gv.array_vaild(index))
		{
			lj->pushnil();
			return 1;
		}
	
		jass_get_global_variable(lj, jass::opcode_type_remove_array(gv.type()), gv[index]);
		return 1;
	}
	
	int array_newindex(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;
		jass::global_variable gv = array_value(lj);
		int32_t index = lj->checkinteger(2);
	
		if (!gv.array_vaild(index))
		{
			return 0;
		}
	
		gv[index] = jass_read(lj, jass::opcode_type_to_var_type(gv.type()), 3);
		return 0;
	}
	
	int array_len(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;
		jass::global_variable gv((hashtable::variable_node*)lj->checkunsigned(lua_upvalueindex(1)));
		lj->pushinteger(gv.array_size());
		return 1;
	}

	int array_create(lua_State* L, uintptr_t value)
	{
		lua_newtable(L);
		luaL_getmetatable(L, LUA_JASS_ARRAY);
		lua_setmetatable(L, -2);
		lua_pushstring(L, "__value");
		lua_pushunsigned(L, value);
		lua_rawset(L, -3);
		return 1;
	}

	void array_make_mt(lua_State* L)
	{
		luaL_Reg lib[] = {
			{ "__index", array_index },
			{ "__newindex", array_newindex },
			{ "__len", array_len },
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
