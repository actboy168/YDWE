#include "jassbind.h"
#include "class_array.h"
#include "libs_runtime.h"
#include "common.h"
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/jass/func_value.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/global_variable.h>
#include <base/lua/private.h> 
#include <base/lua/make_range.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	void jass_get_global_variable(lua::state* ls, jass::OPCODE_VARIABLE_TYPE opt, uint32_t value)
	{
		switch (opt)
		{
		case jass::OPCODE_VARIABLE_NOTHING:
		case jass::OPCODE_VARIABLE_UNKNOWN:
		case jass::OPCODE_VARIABLE_NULL:
			ls->pushnil();
			break;
		case jass::OPCODE_VARIABLE_CODE:
			jassbind::push_code(ls, value);
			break;
		case jass::OPCODE_VARIABLE_INTEGER:
			jassbind::push_integer(ls, value);
			break;
		case jass::OPCODE_VARIABLE_REAL:
			jassbind::push_real(ls, value);
			break;
		case jass::OPCODE_VARIABLE_STRING:
			jassbind::push_string(ls, get_jass_vm()->string_table->get(value));
			break;
		case jass::OPCODE_VARIABLE_HANDLE:
			jassbind::push_handle(ls, value);
			break;
		case jass::OPCODE_VARIABLE_BOOLEAN:
			jassbind::push_boolean(ls, value);
			break;
		default:
			ls->pushnil();
			break;
		}
	}

	int jass_get_global_variable(lua::state* ls, const jass::global_variable& gv)
	{
		if (gv.is_vaild())
		{
			if (!gv.is_array())
			{
				jass_get_global_variable(ls, gv.type(), gv);
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
					return jarray_create(ls->self(), (uintptr_t)gv.ptr());
				default:
					ls->pushnil();
					return 1;
				}
			}
		}
		ls->pushnil();
		return 1;
	}
}}}

namespace base { namespace lua {
	template <>
	inline int convert_to_lua(state* ls, const warcraft3::hashtable::variable_node& v)
	{
		warcraft3::jass::global_variable gv(const_cast<warcraft3::hashtable::variable_node*>(&v));
		ls->pushstring(gv.name());
		return 1 + warcraft3::lua_engine::jass_get_global_variable(ls, gv);
	}
}}

namespace base { namespace warcraft3 { namespace lua_engine {

	int jglobals_get(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;
		if (!is_gaming())
		{
			ls->pushnil();
			return 1;
		}

		const char* name = ls->tostring(2);
		jass::global_variable gv(name);
		return jass_get_global_variable(ls, gv);
	}

	int jglobals_set(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;

		if (!is_gaming())
		{
			return 0;
		}

		const char* name = ls->tostring(2);
		jass::global_variable gv(name);
		if (gv.is_vaild())
		{
			if (!gv.is_array())
			{
				gv = jass_read(ls, jass::opcode_type_to_var_type(gv.type()), 3);
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
		return lua::make_range((lua::state*)L, vt);
	}

	int jass_globals(lua::state* ls)
	{
		ls->newtable();
		{
			ls->newtable();
			{
				ls->pushstring("__index");
				ls->pushcclosure((lua::cfunction)jglobals_get, 0);
				ls->rawset(-3);

				ls->pushstring("__newindex");
				ls->pushcclosure((lua::cfunction)jglobals_set, 0);
				ls->rawset(-3);

				ls->pushstring("__pairs");
				ls->pushcclosure((lua::cfunction)jglobals_pairs, 0);
				ls->rawset(-3);
			}
			ls->setmetatable(-2);
		}
		return 1;
	}
}}}
