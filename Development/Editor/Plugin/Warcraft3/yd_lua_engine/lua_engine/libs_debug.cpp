#include "lua_helper.h"	   
#include <base/warcraft3/jass/func_value.h>	   
#include <base/warcraft3/jass/global_variable.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	static int functiondef(lua_State* L)
	{
		lua_getupvalue(L, 1, 1);
		uintptr_t ptr = (uintptr_t)lua_tounsigned(L, -1);
		const warcraft3::jass::func_value* nf = (const warcraft3::jass::func_value*)ptr;
		if (nf && nf->is_valid())
		{
			lua_newtable(L);
			lua_pushlstring(L, (const char*)&(nf->get_return()), 1);
			lua_rawseti(L, -2, 1);
			for (size_t i = 0; i < nf->get_param().size(); ++i)
			{
				lua_pushlstring(L, (const char*)&(nf->get_param()[i]), 1);
				lua_rawseti(L, -2, i + 2);
			}
			return 1;
		}
		lua_pushnil(L);
		return 1;
	}

	static int globaldef(lua_State* L)
	{
		const char* name = lua_tostring(L, 1);
		jass::global_variable gv(name);
		if (gv.is_vaild())
		{
			char tmp[4] = { 0 };
			tmp[0] = (char)jass::opcode_type_to_var_type(gv.type());
			if (gv.is_array())
			{
				tmp[1] = '[';
				tmp[2] = ']';
			}
			lua_pushstring(L, tmp);
			return 1;
		}
		lua_pushnil(L);
		return 1;
	}

	int jass_debug(lua::state* ls)
	{
		lua_State* L = ls->self();
		lua_newtable(L);
		{
			luaL_Reg lib[] = {
				{ "functiondef", functiondef },
				{ "globaldef", globaldef },
				{ NULL, NULL },
			};
			luaL_setfuncs(L, lib, 0);
		}
		return 1;
	}
}}}
