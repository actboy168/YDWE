#include "../lua/jassbind.h"
#include "../lua/callback.h"
#include "../main/jump_func.h"
#include <ydwe/warcraft3/hashtable.h>
#include <ydwe/warcraft3/war3_searcher.h>
#include <ydwe/warcraft3/native_function.h>
#include <ydwe/warcraft3/jass.h>
#include <ydwe/util/singleton.h>

namespace ydwe { namespace warcraft3 { namespace lua_engine {

	bool is_gaming()
	{
		return get_war3_searcher().is_gaming();
	}

	int jass_call_closure(lua_State* L)
	{
		lua::jassbind* lj = (lua::jassbind*)L;

		if (!is_gaming())
		{
			lj->pushnil();
			return 1;
		}

		native_function::native_function* nf = (native_function::native_function*)lj->tounsigned(lua_upvalueindex(1));

		size_t param = nf->get_param().size();

		if ((int)param > lj->gettop())
		{
			lj->pushnil();
			return 1;
		}

		std::unique_ptr<uintptr_t[]>                    buffer(new uintptr_t[param]);
		std::vector<jass::jreal_t>                      real_buffer(param);
		std::vector<std::unique_ptr<jass::string_fake>> string_buffer(param);

		for (size_t i = 0; i < param; ++i)
		{
			native_function::variable_type vt = nf->get_param()[i];
			switch (vt)
			{
			case native_function::TYPE_BOOLEAN:
				buffer[i] = lj->read_boolean(i+1);
				break;
			case native_function::TYPE_CODE:
				buffer[i] = (jass::jcode_t)util::singleton_nonthreadsafe<jump_func>::instance().create(lua::callback(lj, i+1), 'YDWE');
				break;
			case native_function::TYPE_HANDLE:
				buffer[i] = lj->read_handle(i+1);
				break;
			case native_function::TYPE_INTEGER:
				buffer[i] = lj->read_integer(i+1);
				break;
			case native_function::TYPE_REAL:
				real_buffer[i] = lj->read_real(i+1);
				buffer[i] = (uintptr_t)&real_buffer[i];
				break;
			case native_function::TYPE_STRING:				
				string_buffer[i].reset(new jass::string_fake(lj->tostring(i+1)));
				buffer[i] = (jass::jstring_t)*string_buffer[i];
				break;
			default:
				assert(false);
				buffer[i] = 0;
				break;
			}
		}

		uintptr_t retval = nf->call(buffer.get());

		switch (nf->get_return())
		{
		case native_function::TYPE_NOTHING:
			return 0;
		case native_function::TYPE_BOOLEAN:
			lj->push_boolean(retval);
			return 1;
		case native_function::TYPE_CODE:
			lj->push_code(retval);
			return 1;
		case native_function::TYPE_HANDLE:
			lj->push_handle(retval);
			return 1;
		case native_function::TYPE_INTEGER:
			lj->push_integer(retval);
			return 1;
		case native_function::TYPE_REAL:
			lj->push_real(retval);
			return 1;
		case native_function::TYPE_STRING:
			lj->push_string(retval);
			return 1;
		default:
			assert(false);
			break;
		}

		return 0;
	}

	int jass_get(lua_State* L)
	{
		lua::jassbind* lj = (lua::jassbind*)L;

		const char* name = lj->tostring(2);

		native_function::native_function const* nf = native_function::jass_func(name);
		if (nf)
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

		hashtable::variable_node* node_ptr = get_variable_hashtable()->get(name);
		if (node_ptr)
		{
			switch (node_ptr->type_)
			{
			case jass::OPCODE_VARIABLE_NOTHING:
			case jass::OPCODE_VARIABLE_UNKNOWN:
			case jass::OPCODE_VARIABLE_NULL:
				lj->pushnil();
				return 1;
			case jass::OPCODE_VARIABLE_CODE:
				lj->push_code(node_ptr->value_);
				return 1;
			case jass::OPCODE_VARIABLE_INTEGER:
				lj->push_integer(node_ptr->value_);
				return 1;
			case jass::OPCODE_VARIABLE_REAL:
				lj->push_real(node_ptr->value_);
				return 1;
			case jass::OPCODE_VARIABLE_STRING:
				lj->push_string(node_ptr->value_);
				return 1;
			case jass::OPCODE_VARIABLE_HANDLE:
				lj->push_handle(node_ptr->value_);
				return 1;
			case jass::OPCODE_VARIABLE_BOOLEAN:
				lj->push_boolean(node_ptr->value_);
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
		lua::jassbind* lj = (lua::jassbind*)L;

		if (!is_gaming())
		{
			return 0;
		}

		const char* name = lj->tostring(2);
		hashtable::variable_node* ptr = get_variable_hashtable()->get(name);
		if (ptr)
		{
			switch (ptr->type_)
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
				ptr->value_ = lj->read_integer(3);
				break;
			case jass::OPCODE_VARIABLE_REAL:
				ptr->value_ = lj->read_real(3);
				break;
			case jass::OPCODE_VARIABLE_STRING:
				//
				// Fixed me
				//
				break;
			case jass::OPCODE_VARIABLE_HANDLE:
				ptr->value_ = lj->read_handle(3);
				break;
			case jass::OPCODE_VARIABLE_BOOLEAN:
				ptr->value_ = lj->read_boolean(3);
				break;
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

	int handle_eq(lua_State *L)
	{
		lua::jassbind* lj = (lua::jassbind*)L;
		jass::jhandle_t a = lj->read_handle(1);
		jass::jhandle_t b = lj->read_handle(2);
		lj->pushboolean(a == b);
		return 1;
	}

	int handle_tostring(lua_State *L)
	{
		static char hex[] = "0123456789ABCDEF";

		lua::jassbind* lj = (lua::jassbind*)L;
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
}}}
