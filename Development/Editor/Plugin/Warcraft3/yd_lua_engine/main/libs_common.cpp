#include "../lua/jassbind.h"
#include "class_handle.h"
#include "class_array.h"
#include "libs_runtime.h"
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/jass/func_value.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/global_variable.h>
#include <base/warcraft3/jass/trampoline_function.h>
#include <base/util/singleton.h>
#include <base/lua/private.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	bool is_gaming()
	{
		return 0 != get_jass_virtual_machine();
	}

	bool jass_push(jassbind* lj, jass::variable_type vt, uint32_t value)
	{
		switch (vt)
		{
		case jass::TYPE_NOTHING:
			return false;
		case jass::TYPE_BOOLEAN:
			lj->push_boolean(value);
			return true;
		case jass::TYPE_CODE:
			lj->push_code(value);
			return true;
		case jass::TYPE_HANDLE:
			lj->push_handle(value);
			return true;
		case jass::TYPE_INTEGER:
			lj->push_integer(value);
			return true;
		case jass::TYPE_REAL:
			lj->push_real(value);
			return true;
		case jass::TYPE_STRING:
			lj->push_string(value);
			return true;
		default:
			assert(false);
			return false;
		}
	}

	uintptr_t jass_read(jassbind* lj, jass::variable_type opt, int idx)
	{
		switch (opt)
		{
		case jass::TYPE_NOTHING:
			return 0;
		case jass::TYPE_CODE:
			return lj->read_code(idx);
		case jass::TYPE_INTEGER:
			return lj->read_integer(idx);
		case jass::TYPE_REAL:
			return lj->read_real(idx);
		case jass::TYPE_STRING:
			return lj->read_string(idx);
		case jass::TYPE_HANDLE:
			return lj->read_handle(idx);
		case jass::TYPE_BOOLEAN:
			return lj->read_boolean(idx);
		default:
			assert(false);
			return 0;
		}
	}

	uintptr_t safe_jass_call(lua_State* L, uintptr_t func_address, const uintptr_t* param_list, size_t param_list_size)
	{
		__try {
			return jass::call(func_address, param_list, param_list_size);
		}
		__except (EXCEPTION_EXECUTE_HANDLER){
			lua_pushstring(L, "Call jass function crash.");
			lua_error(L);
		}

		return 0;
	}

	int jass_call_native_function(jassbind* lj, const jass::func_value* nf, uintptr_t func_address = 0)
	{
		size_t param_size = nf->get_param().size();

		if ((int)param_size > lj->gettop())
		{
			lj->pushnil();
			return 1;
		}

		jass::call_param param(param_size);

		for (size_t i = 0; i < param_size; ++i)
		{
			jass::variable_type vt = nf->get_param()[i];
			switch (vt)
			{
			case jass::TYPE_BOOLEAN:
				param.push(i, lj->read_boolean(i+1));
				break;
			case jass::TYPE_CODE:
				param.push(i, lj->read_code(i+1));
				break;
			case jass::TYPE_HANDLE:
				param.push(i, lj->read_handle(i+1));
				break;
			case jass::TYPE_INTEGER:
				param.push(i, lj->read_integer(i+1));
				break;
			case jass::TYPE_REAL:
				param.push_real(i, lj->read_real(i+1));
				break;
			case jass::TYPE_STRING:				
				param.push(i, lj->tostring(i+1));
				break;
			default:
				param.push(i, 0);
				break;
			}
		}

		if (func_address == 0) func_address = nf->get_address();

		uintptr_t retval = 0;
		if (runtime::catch_crash)
		{
			retval = safe_jass_call(lj->self(), func_address, param.data(), param_size);
		}
		else
		{
			retval = jass::call(func_address, param.data(), param_size);
		}


		if (nf->get_return() == jass::TYPE_STRING)
		{
			retval = get_string_fasttable()->get(retval);
		}

		return jass_push(lj, nf->get_return(), retval) ? 1: 0;
	}

	int jass_call_closure(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;

		if (!is_gaming())
		{
			lj->pushnil();
			return 1;
		}

		int result = jass_call_native_function(lj, (const jass::func_value*)lj->tounsigned(lua_upvalueindex(1)));

		if (lua::allow_yield(lj->self()))
		{
			uintptr_t vm = get_current_jass_virtual_machine();
			if (vm && *(uintptr_t*)(vm + 0x34))
			{
				*(uintptr_t*)(vm + 0x20) -= jass::trampoline_size();
				return lua_yield(lj->self(), 0);
			}
		}

		return result;
	}

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
			lj->push_string(get_string_fasttable()->get(value));
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
