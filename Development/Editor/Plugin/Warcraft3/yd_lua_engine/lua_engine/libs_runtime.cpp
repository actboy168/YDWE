#include <lua.hpp>
#include <base/util/console.h>
#include <cstring>
#include "libs_runtime.h" 
#include "allow_yield.h"

namespace base { namespace warcraft3 { namespace lua_engine { namespace runtime	{
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

		int set_err_function(lua_State* L, int index)
		{
			if (lua_isfunction(L, index) || lua_isnil(L, index))
			{
				lua_pushvalue(L, index);
				lua_setfield(L, LUA_REGISTRYINDEX, "_JASS_ERROR_HANDLE");
			}
			return 0;
		}

		int get_err_function(lua_State* L)
		{
			lua_getfield(L, LUA_REGISTRYINDEX, "_JASS_ERROR_HANDLE");
			return 1;
		}

		int get_global_table(lua_State* L, const char* name, bool weak)
		{
			lua_getfield(L, LUA_REGISTRYINDEX, name);
			if (!lua_istable(L, -1))
			{
				lua_pop(L, 1);
				lua_newtable(L);
				if (weak)
				{
					lua_newtable(L);
					{
						lua_pushstring(L, "__mode");
						lua_pushstring(L, "kv");
						lua_rawset(L, -3);
					}
					lua_setmetatable(L, -2);
				}
				lua_pushvalue(L, -1);
				lua_setfield(L, LUA_REGISTRYINDEX, name);
			}
			return 1;
		}

		int thread_get_table(lua_State* L)
		{
			return get_global_table(L, "_JASS_THREAD_TABLE", true);
		}

		int thread_create(lua_State* L, int index)
		{
			thread_get_table(L);
			lua_pushvalue(L, index);
			lua_rawget(L, -2);
			if (lua_isnil(L, -1))
			{
				lua_pop(L, 1);
				lua_newthread(L);
			}
			else
			{
				lua_pushvalue(L, index);
				lua_pushnil(L);
				lua_rawset(L, -4);
			}
			lua_remove(L, -2);
			return 1;
		}

		int thread_save(lua_State* L, int key, int value)
		{
			thread_get_table(L);
			lua_pushvalue(L, key);
			lua_pushvalue(L, value);
			lua_rawset(L, -3);
			lua_pop(L, 1);
			return 0;
		}

		int handle_ud_get_table(lua_State* L)
		{
			return get_global_table(L, "_JASS_HANDLE_UD_TABLE", true);
		}

		int callback_get_table(lua_State* L)
		{
			return get_global_table(L, "_JASS_CALLBACK_TABLE", false);
		}

		int callback_push(lua_State* L, int idx)
		{
			callback_get_table(L);
			
			// read t[v]
			lua_pushvalue(L, idx);
			lua_rawget(L, -2);
			if (lua_isnumber(L, -1))
			{
				int ret = lua_tointeger(L, -1);
				lua_pop(L, 2);
				return ret;
			}
			lua_pop(L, 1);

			int free = 1 + (int)lua_rawlen(L, -1);

			// t[free] = v
			lua_pushvalue(L, idx);
			lua_rawseti(L, -2, free);

			// t[v] = free
			lua_pushvalue(L, idx);
			lua_pushinteger(L, free);
			lua_rawset(L, -3);

			// pop t
			lua_pop(L, 1);
			return free;
		}

		int callback_read(lua_State* L, int ref)
		{
			callback_get_table(L);
			lua_rawgeti(L, -1, ref);
			lua_remove(L, -2);
			return 1;
		}


	int jass_runtime_set(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);

		if (strcmp("error_handle", name) == 0)
		{
			set_err_function(L, 3);
		}
		else if (strcmp("handle_level", name) == 0)
		{
			handle_level = luaL_checkinteger(L, 3);
		}
		else if (strcmp("console", name) == 0)
		{
			enable_console = !!lua_toboolean(L, 3);
			if (enable_console)
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
			sleep = !!lua_toboolean(L, 3);
		}
		else if (strcmp("catch_crash", name) == 0)
		{
			catch_crash = !!lua_toboolean(L, 3);
		}
		
		return 0;
	}

	int jass_runtime_get(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);

		if (strcmp("version", name) == 0)
		{
			lua_pushinteger(L, version);
			return 1;
		}
		else if (strcmp("error_handle", name) == 0)
		{
			return get_err_function(L);
		}
		else if (strcmp("handle_level", name) == 0)
		{
			lua_pushinteger(L, handle_level);
			return 1;
		}
		else if (strcmp("console", name) == 0)
		{
			lua_pushboolean(L, enable_console);
			return 1;
		}
		else if (strcmp("sleep", name) == 0)
		{
			lua_pushboolean(L, sleep);
			return 1;
		}
		else if (strcmp("can_sleep", name) == 0)
		{
			lua_pushboolean(L, sleep && lua::allow_yield(L));
			return 1;
		}
		else if (strcmp("catch_crash", name) == 0)
		{
			lua_pushboolean(L, catch_crash);
			return 1;
		}

		return 0;
	}

	int open(lua_State* L)
	{
		lua_newtable(L);
		{
			lua_newtable(L);
			{
				lua_pushstring(L, "__index");
				lua_pushcclosure(L, jass_runtime_get, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__newindex");
				lua_pushcclosure(L, jass_runtime_set, 0);
				lua_rawset(L, -3);
			}
			lua_setmetatable(L, -2);
		}
		return 1;
	}

}}}}
