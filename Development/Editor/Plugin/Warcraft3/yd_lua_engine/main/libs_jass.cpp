#include "../lua/jassbind.h"
#include "handle.h"
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/jass/func_value.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/global_variable.h>
#include <base/util/singleton.h>

namespace base { namespace warcraft3 { namespace lua_engine {

#define LUA_JASS_ARRAY "jarray_t"

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
		uintptr_t retval = jass::call(func_address, param.data(), param_size);

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

		return jass_call_native_function(lj, (const jass::func_value*)lj->tounsigned(lua_upvalueindex(1)));
	}

	int jass_call_null_function(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;
		printf("Wanring: %s\n", lj->tostring(lua_upvalueindex(1)));
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
			if (0 == strcmp(name, "TriggerSleepAction"))
			{
				lj->pushstring("TriggerSleepAction isn't implemented.");
				lj->pushcclosure((lua::state::cfunction)jass_call_null_function, 1);
				return 1;
			}
			else
			{
				lj->pushunsigned((uint32_t)(uintptr_t)nf);
				lj->pushcclosure((lua::state::cfunction)jass_call_closure, 1);
				return 1;
			}
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
					lj->newtable();
					luaL_getmetatable(lj->self(), LUA_JASS_ARRAY);
					lj->setmetatable(-2);
					lj->pushstring("__value");
					lj->pushunsigned((uintptr_t)gv.ptr());
					lj->rawset(-3);
					return 1;
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

	void array_make_mt(lua::state* ls)
	{
		luaL_Reg lib[] = {
			{ "__index",    array_index },
			{ "__newindex", array_newindex },
			{ "__len",      array_len },
			{ NULL, NULL },
		};

		luaL_newmetatable(ls->self(), LUA_JASS_ARRAY);
#if LUA_VERSION_NUM >= 502
		luaL_setfuncs(ls->self(), lib, 0);
		ls->pop(1);
#else
		luaL_register(ls->self(), NULL, lib);
#endif
	}

	int open_jass(lua::state* ls)
	{
		handle_lud_make_mt(ls); 
		handle_ud_make_mt(ls);
		array_make_mt(ls);
		
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
		ls->setglobal("jass");

		return 1;
	}
}}}
