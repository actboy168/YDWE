#include <base/lua/state.h>
#include <base/util/console.h>
#include <cstring>
#include "runtime.h"

namespace base { namespace warcraft3 { namespace lua_engine {

	namespace runtime
	{
		int  error_handle = 0;
		int  handle_level = 1;
		int  handle_ud_table = 0;
		bool console = false;

		void initialize()
		{
			error_handle = 0;
			handle_level = 1;
			handle_ud_table = 0;
			console = false;
		}

		void set_function(int& result, lua_State* L, int index)
		{
			if (lua_isfunction(L, index))
			{
				if (result != 0)
				{
					luaL_unref(L, LUA_REGISTRYINDEX, result);
				}

				lua_pushvalue(L, index);
				result = luaL_ref(L, LUA_REGISTRYINDEX);
			}
			else if (lua_isnil(L, index))
			{
				if (result != 0)
				{
					luaL_unref(L, LUA_REGISTRYINDEX, result);
				}

				result = 0;
			}
		}

		void get_function(int result, lua_State* L)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, result);
		}
	}

	int jass_runtime_set(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;
		const char* name = ls->tostring(2);

		if (strcmp("error_handle", name) == 0)
		{
			runtime::set_function(runtime::error_handle, ls->self(), 3);
		}
		else if (strcmp("handle_level", name) == 0)
		{
			runtime::handle_level = ls->checkinteger(3);
		}
		else if (strcmp("console", name) == 0)
		{
			runtime::console = !!ls->toboolean(3);
			if (runtime::console)
			{
				console::enable();
				console::disable_close_button();
			}
			else
			{
				console::disable();
			}
		}
		
		return 0;
	}

	int jass_runtime_get(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;
		const char* name = ls->tostring(2);

		if (strcmp("error_handle", name) == 0)
		{
			runtime::get_function(runtime::error_handle, ls->self());
			return 1;
		}
		else if (strcmp("handle_level", name) == 0)
		{
			ls->pushinteger(runtime::handle_level);
			return 1;
		}
		else if (strcmp("console", name) == 0)
		{
			ls->pushboolean(runtime::console);
			return 1;
		}

		return 0;
	}
}}}
