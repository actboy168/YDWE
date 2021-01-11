#include "jassbind.h" 
#include <warcraft3/jass/func_value.h>	   
#include <warcraft3/jass/global_variable.h>
#include <warcraft3/war3_searcher.h>
#include <fmt/format.h>

namespace warcraft3::lua_engine::debug {

	static int functiondef(lua_State* L)
	{
		lua_getupvalue(L, 1, 1);
		uintptr_t ptr = (uintptr_t)lua_tointeger(L, -1);
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

	static int handledef(lua_State* L)
	{
		jass::jhandle_t h = 0;
		if (lua_type(L, 1) == LUA_TNUMBER)
		{
			h = lua_tointeger(L, 1);
		}
		else
		{
			h = jassbind::read_handle(L, 1);
		}

		if (h < 0x100000)
		{
			lua_pushnil(L);
			return 1;
		}
		handle_table_t** hts = get_jass_vm()->handle_table;
		if (!hts) 
		{
			lua_pushnil(L);
			return 1;
		}
		hashtable::reverse_table& table = (*hts)->table;
		uint32_t object = (uint32_t)table.at(3 * (h - 0x100000) + 1);
		uint32_t reference = (uint32_t)table.at(3 * (h - 0x100000));
	
		lua_newtable(L);
		{
			lua_pushliteral(L, "reference");
			lua_pushinteger(L, reference);
			lua_rawset(L, -3);

			if (object)
			{
				char typestr[4] = { 0 };
				uint32_t type = get_object_type(object);
				typestr[0] = ((const char*)&type)[3];
				typestr[1] = ((const char*)&type)[2];
				typestr[2] = ((const char*)&type)[1];
				typestr[3] = ((const char*)&type)[0];
				lua_pushliteral(L, "type");
				lua_pushlstring(L, typestr, 4);
				lua_rawset(L, -3);
			}
		}
		return 1;
	}

	static int currentpos(lua_State* L)
	{
		jass::opcode* current_op = warcraft3::jass::currentpos();
		jass::opcode* op;
		for (op = current_op; op->op != jass::OPTYPE_FUNCTION; --op)
		{
		}

		lua_pushstring(L, jass::from_stringid(op->arg));
		lua_pushinteger(L, current_op - op);
		return 2;
	}

	static int handlemax(lua_State* L)
	{
		handle_table_t** hts = get_jass_vm()->handle_table;
		if (hts) {
			hashtable::reverse_table& table = (*hts)->table;
			lua_pushinteger(L, table.size);
			return 1;
		}
		lua_pushinteger(L, 0);
		return 1;
	}

	static int handlecount(lua_State* L)
	{
		handle_table_t** hts = get_jass_vm()->handle_table;
		if (hts) {
			hashtable::reverse_table& table = (*hts)->table;
			uint32_t n = 0;
			for (uint32_t i = 1; i < table.size * 3; i += 3)
			{
				if (0 != (uintptr_t)table.at(i))
				{
					n++;
				}
			}
			lua_pushinteger(L, n);
			return 1;
		}
		lua_pushinteger(L, 0);
		return 1;
	}

	static int h2i(lua_State* L)
	{
		lua_pushinteger(L, jassbind::read_handle(L, 1));
		return 1;
	}

	static int i2h(lua_State* L)
	{
		jassbind::push_handle(L, lua_tointeger(L, 1));
		return 1;
	}

	static int handle_ref(lua_State* L)
	{
		jass::handle_ref(jassbind::read_handle(L, 1));
		return 1;
	}

	static int handle_unref(lua_State* L)
	{
		jass::handle_unref(jassbind::read_handle(L, 1));
		return 1;
	}

	static int gchash(lua_State* /*L*/)
	{
		// 已经没用了
		return 0;
	}

	int open(lua_State* L)
	{
		lua_newtable(L);
		{
			luaL_Reg lib[] = {
				{ "functiondef", functiondef },
				{ "globaldef", globaldef },
				{ "handledef", handledef },
				{ "currentpos", currentpos },
				{ "handlemax", handlemax },
				{ "handlecount", handlecount },
				{ "h2i", h2i },
				{ "i2h", i2h },
				{ "handle_ref", handle_ref },
				{ "handle_unref", handle_unref },
				{ "gchash", gchash },
				{ NULL, NULL },
			};
			luaL_setfuncs(L, lib, 0);
		}
		return 1;
	}
}
