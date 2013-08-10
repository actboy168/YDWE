#include "../lua/jassbind.h"
#include "../lua/callback.h"
#include <ydwe/warcraft3/hashtable.h>
#include <ydwe/warcraft3/war3_searcher.h>
#include <ydwe/warcraft3/native_function.h>
#include <ydwe/warcraft3/jass.h>
#include <ydwe/warcraft3/jass/global_variable.h>
#include <ydwe/util/singleton.h>

_BASE_BEGIN
namespace warcraft3 { namespace lua_engine {

	bool is_gaming()
	{
		return get_war3_searcher().is_gaming();
	}

	bool jass_push(jassbind* lj, native_function::variable_type vt, uint32_t value)
	{
		switch (vt)
		{
		case native_function::TYPE_NOTHING:
			return false;
		case native_function::TYPE_BOOLEAN:
			lj->push_boolean(value);
			return true;
		case native_function::TYPE_CODE:
			lj->push_code(value);
			return true;
		case native_function::TYPE_HANDLE:
			lj->push_handle(value);
			return true;
		case native_function::TYPE_INTEGER:
			lj->push_integer(value);
			return true;
		case native_function::TYPE_REAL:
			lj->push_real(value);
			return true;
		case native_function::TYPE_STRING:
			lj->push_string(value);
			return true;
		default:
			assert(false);
			return false;
		}
	}

	int jass_call_native_function(jassbind* lj, const native_function::native_function* nf, uintptr_t func_address = 0)
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
			native_function::variable_type vt = nf->get_param()[i];
			switch (vt)
			{
			case native_function::TYPE_BOOLEAN:
				param.push(i, lj->read_boolean(i+1));
				break;
			case native_function::TYPE_CODE:
				param.push(i, (jass::jcode_t)cfunction_to_code(lj, i+1));
				break;
			case native_function::TYPE_HANDLE:
				param.push(i, lj->read_handle(i+1));
				break;
			case native_function::TYPE_INTEGER:
				param.push(i, lj->read_integer(i+1));
				break;
			case native_function::TYPE_REAL:
				param.push(i, (float)lj->tonumber(i+1));
				break;
			case native_function::TYPE_STRING:				
				param.push(i, lj->tostring(i+1));
				break;
			default:
				param.push(i, 0);
				break;
			}
		}

		if (func_address == 0) func_address = nf->get_address();
		uintptr_t retval = jass::call(func_address, param.data(), param_size);

		if (nf->get_return() == native_function::TYPE_STRING)
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

		return jass_call_native_function(lj, (const native_function::native_function*)lj->tounsigned(lua_upvalueindex(1)));
	}

	int jass_get(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;

		const char* name = lj->tostring(2);

		native_function::native_function const* nf = native_function::jass_func(name);
		if (nf && nf->is_valid())
		{
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
			switch (gv.type())
			{
			case jass::OPCODE_VARIABLE_NOTHING:
			case jass::OPCODE_VARIABLE_UNKNOWN:
			case jass::OPCODE_VARIABLE_NULL:
				lj->pushnil();
				return 1;
			case jass::OPCODE_VARIABLE_CODE:
				lj->push_code(gv);
				return 1;
			case jass::OPCODE_VARIABLE_INTEGER:
				lj->push_integer(gv);
				return 1;
			case jass::OPCODE_VARIABLE_REAL:
				lj->push_real(gv);
				return 1;
			case jass::OPCODE_VARIABLE_STRING:
				lj->push_string(gv);
				return 1;
			case jass::OPCODE_VARIABLE_HANDLE:
				lj->push_handle(gv);
				return 1;
			case jass::OPCODE_VARIABLE_BOOLEAN:
				lj->push_boolean(gv);
				return 1;
			case jass::OPCODE_VARIABLE_INTEGER_ARRAY:
			case jass::OPCODE_VARIABLE_REAL_ARRAY:
			case jass::OPCODE_VARIABLE_STRING_ARRAY:
			case jass::OPCODE_VARIABLE_HANDLE_ARRAY:
			case jass::OPCODE_VARIABLE_BOOLEAN_ARRAY:
				//
				// Fixed me
				//
				lj->pushnil();
				return 1;
			default:
				return 1;
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
			switch (gv.type())
			{
			case jass::OPCODE_VARIABLE_NOTHING:
			case jass::OPCODE_VARIABLE_UNKNOWN:
			case jass::OPCODE_VARIABLE_NULL:
				break;
			case jass::OPCODE_VARIABLE_CODE:
				//
				// Fixed me
				//
				break;
			case jass::OPCODE_VARIABLE_INTEGER:
				gv = lj->read_integer(3);
				break;
			case jass::OPCODE_VARIABLE_REAL:
				gv = lj->read_real(3);
				break;
			case jass::OPCODE_VARIABLE_STRING:
				//
				// Fixed me
				//
				break;
			case jass::OPCODE_VARIABLE_HANDLE:
				gv = lj->read_handle(3);
				break;
			case jass::OPCODE_VARIABLE_BOOLEAN:
				gv = lj->read_boolean(3);
				break;
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

		return 0;
	}

	int open_jass(lua::state* ls)
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
		ls->setglobal("jass");

		return 1;
	}
}}

_BASE_END
