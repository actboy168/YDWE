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

namespace base { namespace warcraft3 { namespace lua_engine {

	int jass_call_null_function(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;
		printf("Wanring: %s isn't implemented.\n", lj->tostring(lua_upvalueindex(1)));
		lj->pushnil();
		return 1;
	}

	void jass_get_global_variable(jassbind* lj, jass::OPCODE_VARIABLE_TYPE opt, uint32_t value)
	{
		switch (opt)
		{
		case jass::OPCODE_VARIABLE_NOTHING:
		case jass::OPCODE_VARIABLE_UNKNOWN:
		case jass::OPCODE_VARIABLE_NULL:
			lj->pushnil();
			break;
		case jass::OPCODE_VARIABLE_CODE:
			lj->push_code(value);
			break;
		case jass::OPCODE_VARIABLE_INTEGER:
			lj->push_integer(value);
			break;
		case jass::OPCODE_VARIABLE_REAL:
			lj->push_real(value);
			break;
		case jass::OPCODE_VARIABLE_STRING:
			lj->push_string(get_jass_vm()->string_table->get(value));
			break;
		case jass::OPCODE_VARIABLE_HANDLE:
			lj->push_handle(value);
			break;
		case jass::OPCODE_VARIABLE_BOOLEAN:
			lj->push_boolean(value);
			break;
		default:
			lj->pushnil();
			break;
		}
	}

	int jass_get(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;

		const char* name = lj->tostring(2);

		jass::func_value const* nf = jass::jass_func(name);
		if (nf && nf->is_valid())
		{
			if (!lua::allow_yield(lj->self()))
			{
				if ((0 == strcmp(name, "TriggerSleepAction"))
					|| (0 == strcmp(name, "TriggerWaitForSound"))
					|| (0 == strcmp(name, "TriggerSyncReady"))
					|| (0 == strcmp(name, "SyncSelections")))
				{
					lj->pushstring(name);
					lj->pushcclosure((lua::state::cfunction)jass_call_null_function, 1);
					return 1;
				}
			}

			lj->pushunsigned((uint32_t)(uintptr_t)nf);
			lj->pushcclosure((lua::state::cfunction)jass_call_closure, 1);
			return 1;
		}

		if (!is_gaming())
		{
			lj->pushnil();
			return 1;
		}

		jass::global_variable gv(name);
		if (gv.is_vaild())
		{
			if (!gv.is_array())
			{
				jass_get_global_variable(lj, gv.type(), gv);
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
					return jarray_create(lj->self(), (uintptr_t)gv.ptr());
				default:
					lj->pushnil();
					return 1;
				}
			}
		}

		lj->pushnil();
		return 1;
	}

	int jass_set(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;

		if (!is_gaming())
		{
			return 0;
		}

		const char* name = lj->tostring(2);
		jass::global_variable gv(name);
		if (gv.is_vaild())
		{
			if (!gv.is_array())
			{
				gv = jass_read(lj, jass::opcode_type_to_var_type(gv.type()), 3);
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

	int jass_common(lua::state* ls)
	{
		ls->newtable();
		{
			ls->newtable();
			{
				ls->pushstring("__index");
				ls->pushcclosure((lua::state::cfunction)jass_get, 0);
				ls->rawset(-3);

				ls->pushstring("__newindex");
				ls->pushcclosure((lua::state::cfunction)jass_set, 0);
				ls->rawset(-3);
			}
			ls->setmetatable(-2);
		}
		return 1;
	}
}}}
