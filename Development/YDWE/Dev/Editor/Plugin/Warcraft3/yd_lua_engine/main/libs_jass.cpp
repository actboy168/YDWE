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

	uintptr_t jass_read(jassbind* lj, native_function::variable_type opt, int idx)
	{
		switch (opt)
		{
		case native_function::TYPE_NOTHING:
			return 0;
		case native_function::TYPE_CODE:
			//
			// Fixed me
			//
			return 0;
		case native_function::TYPE_INTEGER:
			return lj->read_integer(idx);
		case native_function::TYPE_REAL:
			return lj->read_real(idx);
		case native_function::TYPE_STRING:
			//
			// Fixed me
			//
			return 0;
		case native_function::TYPE_HANDLE:
			return lj->read_handle(idx);
		case native_function::TYPE_BOOLEAN:
			return lj->read_boolean(idx);
		default:
			assert(false);
			return 0;
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

	int handle_eq(lua_State *L)
	{
		jassbind* lj = (jassbind*)L;
		jass::jhandle_t a = lj->read_handle(1);
		jass::jhandle_t b = lj->read_handle(2);
		lj->pushboolean(a == b);
		return 1;
	}

	int handle_tostring(lua_State *L)
	{
		static char hex[] = "0123456789ABCDEF";

		jassbind* lj = (jassbind*)L;
		jass::jhandle_t h = lj->read_handle(1);

		luaL_Buffer b;
		luaL_buffinitsize(L , &b , 28);
		luaL_addstring(&b, "handle: 0x");

		bool strip = true;
		for (int i = 7; i >= 0; i--) 
		{
			int c = (h >> (i*4)) & 0xF;
			if (strip && c == 0) 
			{
				continue;
			}
			strip = false;
			luaL_addchar(&b, hex[c]);
		}

		if (strip)
		{
			luaL_addchar(&b , '0');
		}

		luaL_pushresult(&b);

		return 1;
	}

	void handle_make_mt(lua::state* ls)
	{
		luaL_Reg lib[] = {
			{ "__eq",       handle_eq },
			{ "__tostring", handle_tostring },
			{ NULL, NULL },
		};

		ls->pushlightuserdata(NULL);
		luaL_newlib(ls->self(), lib);
		ls->setmetatable(-2);
		ls->pop(1);
	}

	int open_jass(lua::state* ls)
	{
		handle_make_mt(ls);
		
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
