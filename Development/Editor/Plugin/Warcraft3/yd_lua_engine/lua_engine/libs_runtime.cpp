#include <base/lua/state.h>
#include <base/util/console.h>
#include <cstring>
#include "libs_runtime.h" 
#include "allow_yield.h"

namespace base { namespace warcraft3 { namespace lua_engine {

	namespace runtime
	{
		int  version = 2;
		int  handle_level = 2;
		bool enable_console = false;
		bool sleep = true;
		bool catch_crash = true;

		void initialize()
		{
			handle_level = 2;
			enable_console = false;
			sleep = true;
			catch_crash = true;
		}

		int set_err_function(lua::state* ls, int index)
		{
			if (ls->isfunction(index) || ls->isnil(index))
			{
				ls->pushvalue(index);
				ls->setfield(LUA_REGISTRYINDEX, "_JASS_ERROR_HANDLE");
			}
			return 0;
		}

		int get_err_function(lua::state* ls)
		{
			ls->getfield(LUA_REGISTRYINDEX, "_JASS_ERROR_HANDLE");
			return 1;
		}

		int get_global_table(lua::state* ls, const char* name, bool weak)
		{
			ls->getfield(LUA_REGISTRYINDEX, name);
			if (!ls->istable(-1))
			{
				ls->pop(1);
				ls->newtable();
				if (weak)
				{
					ls->newtable();
					{
						ls->pushstring("__mode");
						ls->pushstring("kv");
						ls->rawset(-3);
					}
					ls->setmetatable(-2);
				}
				ls->pushvalue(-1);
				ls->setfield(LUA_REGISTRYINDEX, name);
			}
			return 1;
		}

		int thread_get_table(lua::state* ls)
		{
			return get_global_table(ls, "_JASS_THREAD_TABLE", true);
		}

		int thread_create(lua::state* ls, int index)
		{
			thread_get_table(ls);
			ls->pushvalue(index);
			ls->rawget(-2);
			if (ls->isnil(-1))
			{
				ls->pop(1);
				ls->newthread();
			}
			else
			{
				ls->pushvalue(index);
				ls->pushnil();
				ls->rawset(-4);
			}
			ls->remove(-2);
			return 1;
		}

		int thread_save(lua::state* ls, int key, int value)
		{
			thread_get_table(ls);
			ls->pushvalue(key);
			ls->pushvalue(value);
			ls->rawset(-3);
			ls->pop(1);
			return 0;
		}

		int handle_ud_get_table(lua::state* ls)
		{
			return get_global_table(ls, "_JASS_HANDLE_UD_TABLE", true);
		}

		int callback_get_table(lua::state* ls)
		{
			return get_global_table(ls, "_JASS_CALLBACK_TABLE", false);
		}

		int callback_push(lua::state* ls, int idx)
		{
			callback_get_table(ls);
			
			// read t[v]
			ls->pushvalue(idx);
			ls->rawget(-2);
			if (ls->isnumber(-1))
			{
				int ret = ls->tointeger(-1);
				ls->pop(2);
				return ret;
			}
			ls->pop(1);

			// free = t[0] + 1
			ls->rawgeti(-1, 0);
			int free = 1 + ls->tointeger(-1);
			ls->pop(1);

			// t[0] = free
			ls->pushinteger(free);
			ls->rawseti(-2, 0);


			// t[free] = v
			ls->pushvalue(idx);
			ls->rawseti(-2, free);

			// t[v] = free
			ls->pushvalue(idx);
			ls->pushinteger(free);
			ls->rawset(-3);

			// pop t
			ls->pop(1);
			return free;
		}

		int callback_read(lua::state* ls, int ref)
		{
			callback_get_table(ls);
			ls->rawgeti(-1, ref);
			ls->remove(-2);
			return 1;
		}
	}


	int jass_runtime_set(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;
		const char* name = ls->tostring(2);

		if (strcmp("error_handle", name) == 0)
		{
			runtime::set_err_function(ls, 3);
		}
		else if (strcmp("handle_level", name) == 0)
		{
			runtime::handle_level = ls->checkinteger(3);
		}
		else if (strcmp("console", name) == 0)
		{
			runtime::enable_console = !!ls->toboolean(3);
			if (runtime::enable_console)
			{
				console::enable();
				console::disable_close_button();
			}
			else
			{
				console::disable();
			}
		}
		else if (strcmp("sleep", name) == 0)
		{
			runtime::sleep = !!ls->toboolean(3);
		}
		else if (strcmp("catch_crash", name) == 0)
		{
			runtime::catch_crash = !!ls->toboolean(3);
		}
		
		return 0;
	}

	int jass_runtime_get(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;
		const char* name = ls->tostring(2);

		if (strcmp("version", name) == 0)
		{
			ls->pushinteger(runtime::version);
			return 1;
		}
		else if (strcmp("error_handle", name) == 0)
		{
			return runtime::get_err_function(ls);
		}
		else if (strcmp("handle_level", name) == 0)
		{
			ls->pushinteger(runtime::handle_level);
			return 1;
		}
		else if (strcmp("console", name) == 0)
		{
			ls->pushboolean(runtime::enable_console);
			return 1;
		}
		else if (strcmp("sleep", name) == 0)
		{
			ls->pushboolean(runtime::sleep);
			return 1;
		}
		else if (strcmp("can_sleep", name) == 0)
		{
			ls->pushboolean(runtime::sleep && lua::allow_yield(ls));
			return 1;
		}
		else if (strcmp("catch_crash", name) == 0)
		{
			ls->pushboolean(runtime::catch_crash);
			return 1;
		}

		return 0;
	}

	int jass_runtime(lua::state* ls)
	{
		ls->newtable();
		{
			ls->newtable();
			{
				ls->pushstring("__index");
				ls->pushcclosure((lua::cfunction)jass_runtime_get, 0);
				ls->rawset(-3);

				ls->pushstring("__newindex");
				ls->pushcclosure((lua::cfunction)jass_runtime_set, 0);
				ls->rawset(-3);
			}
			ls->setmetatable(-2);
		}
		return 1;
	}

}}}
