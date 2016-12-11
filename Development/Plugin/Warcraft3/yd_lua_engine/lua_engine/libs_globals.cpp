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

namespace base { namespace warcraft3 { namespace lua_engine { namespace globals {

	void jass_get_global_variable(lua_State* L, jass::OPCODE_VARIABLE_TYPE opt, uint32_t value)
	{
		switch (opt)
		{
		case jass::OPCODE_VARIABLE_NOTHING:
		case jass::OPCODE_VARIABLE_UNKNOWN:
		case jass::OPCODE_VARIABLE_NULL:
			lua_pushnil(L);
			break;
		case jass::OPCODE_VARIABLE_CODE:
			jassbind::push_code(L, value);
			break;
		case jass::OPCODE_VARIABLE_INTEGER:
			jassbind::push_integer(L, value);
			break;
		case jass::OPCODE_VARIABLE_REAL:
			jassbind::push_real(L, value);
			break;
		case jass::OPCODE_VARIABLE_STRING:
			jassbind::push_string(L, get_jass_vm()->string_table->get(value));
			break;
		case jass::OPCODE_VARIABLE_HANDLE:
			jassbind::push_handle(L, value);
			break;
		case jass::OPCODE_VARIABLE_BOOLEAN:
			jassbind::push_boolean(L, value);
			break;
		default:
			lua_pushnil(L);
			break;
		}
	}

	int jass_get_global_variable(lua_State* L, const jass::global_variable& gv)
	{
		if (gv.is_vaild())
		{
			if (!gv.is_array())
			{
				jass_get_global_variable(L, gv.type(), gv);
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
}}}}

namespace base { namespace lua {
	template <>
	inline int convert_to_lua(lua_State* L, const warcraft3::hashtable::variable_node& v)
	{
		warcraft3::jass::global_variable gv(const_cast<warcraft3::hashtable::variable_node*>(&v));
		lua_pushstring(L, gv.name());
		return 1 + warcraft3::lua_engine::globals::jass_get_global_variable(L, gv);
	}
}}

namespace base { namespace warcraft3 { namespace lua_engine { namespace globals {

	int jglobals_get(lua_State* L)
	{
		if (!is_gaming())
		{
			lua_pushnil(L);
			return 1;
		}

		const char* name = lua_tostring(L, 2);
		jass::global_variable gv(name);
		return jass_get_global_variable(L, gv);
	}

	int jglobals_set(lua_State* L)
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

	int jglobals_pairs(lua_State* L)
	{
		const hashtable::variable_table& vt = *(get_jass_vm()->global_table);
		return lua::make_range(L, vt);
	}

	int open(lua_State* L)
	{
		lua_newtable(L);
		{
			lua_newtable(L);
			{
				lua_pushstring(L, "__index");
				lua_pushcclosure(L, jglobals_get, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__newindex");
				lua_pushcclosure(L, jglobals_set, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__pairs");
				lua_pushcclosure(L, jglobals_pairs, 0);
				lua_rawset(L, -3);
			}
			lua_setmetatable(L, -2);
		}
		return 1;
	}
}}}}
