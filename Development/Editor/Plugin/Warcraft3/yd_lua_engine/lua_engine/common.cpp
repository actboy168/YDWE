#include "common.h"
#include "jassbind.h"
#include "class_handle.h"
#include "class_array.h"
#include "libs_runtime.h" 
#include "allow_yield.h"
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/trampoline_function.h>
#include <base/util/singleton.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	bool is_gaming()
	{
		return 0 != get_jass_vm();
	}

	bool jass_push(lua::state* ls, jass::variable_type vt, uint32_t value)
	{
		switch (vt)
		{
		case jass::TYPE_NOTHING:
			return false;
		case jass::TYPE_BOOLEAN:
			jassbind::push_boolean(ls, value);
			return true;
		case jass::TYPE_CODE:
			jassbind::push_code(ls, value);
			return true;
		case jass::TYPE_HANDLE:
			jassbind::push_handle(ls, value);
			return true;
		case jass::TYPE_INTEGER:
			jassbind::push_integer(ls, value);
			return true;
		case jass::TYPE_REAL:
			jassbind::push_real(ls, value);
			return true;
		case jass::TYPE_STRING:
			jassbind::push_string(ls, value);
			return true;
		default:
			assert(false);
			return false;
		}
	}

	uintptr_t jass_read(lua::state* ls, jass::variable_type opt, int idx)
	{
		switch (opt)
		{
		case jass::TYPE_NOTHING:
			return 0;
		case jass::TYPE_CODE:
			return jassbind::read_code(ls, idx);
		case jass::TYPE_INTEGER:
			return jassbind::read_integer(ls, idx);
		case jass::TYPE_REAL:
			return jassbind::read_real(ls, idx);
		case jass::TYPE_STRING:
			return jassbind::read_string(ls, idx);
		case jass::TYPE_HANDLE:
			return jassbind::read_handle(ls, idx);
		case jass::TYPE_BOOLEAN:
			return jassbind::read_boolean(ls, idx);
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

	int jass_call_native_function(lua::state* ls, const jass::func_value* nf, uintptr_t func_address)
	{
		size_t param_size = nf->get_param().size();

		if ((int)param_size > ls->gettop())
		{
			ls->pushnil();
			return 1;
		}

		jass::call_param param(param_size);

		for (size_t i = 0; i < param_size; ++i)
		{
			jass::variable_type vt = nf->get_param()[i];
			switch (vt)
			{
			case jass::TYPE_BOOLEAN:
				param.push(i, jassbind::read_boolean(ls, i + 1));
				break;
			case jass::TYPE_CODE:
				param.push(i, jassbind::read_code(ls, i + 1));
				break;
			case jass::TYPE_HANDLE:
				param.push(i, jassbind::read_handle(ls, i + 1));
				break;
			case jass::TYPE_INTEGER:
				param.push(i, jassbind::read_integer(ls, i + 1));
				break;
			case jass::TYPE_REAL:
				param.push_real(i, jassbind::read_real(ls, i + 1));
				break;
			case jass::TYPE_STRING:
				param.push(i, ls->tostring(i + 1));
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
			retval = safe_jass_call(ls->self(), func_address, param.data(), param_size);
		}
		else
		{
			retval = jass::call(func_address, param.data(), param_size);
		}


		if (nf->get_return() == jass::TYPE_STRING)
		{
			retval = get_jass_vm()->string_table->get(retval);
		}

		return jass_push(ls, nf->get_return(), retval) ? 1 : 0;
	}

	int jass_call_closure(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;

		if (!is_gaming())
		{
			ls->pushnil();
			return 1;
		}

		int result = jass_call_native_function(ls, (const jass::func_value*)ls->tounsigned(lua_upvalueindex(1)));

		if (lua::allow_yield(ls))
		{
			uintptr_t vm = (uintptr_t)get_current_jass_vm_nofix();
			if (vm && *(uintptr_t*)(vm + 0x34))
			{
				*(uintptr_t*)(vm + 0x20) -= jass::trampoline_size();
				return lua_yield(ls->self(), 0);
			}
		}

		return result;
	}

	lua::state* get_mainthread(lua::state* thread)
	{
		thread->rawgeti(LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
		lua::state* ml = (lua::state*)thread->tothread(-1);
		thread->pop(1);
		return ml;
	}
}}}
