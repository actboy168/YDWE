#include <lua.hpp>
#include <base/util/console.h>
#include <base/util/unicode.h> 	  
#include "libs_runtime.h"
#include "callback.h"

namespace base {  namespace warcraft3 { namespace lua_engine { namespace console {

	int jass_console_set(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);

		if (strcmp("enable", name) == 0)
		{
			runtime::enable_console = !!lua_toboolean(L, 3);
			if (runtime::enable_console)
			{
				base::console::enable();
				base::console::disable_close_button();
			}
			else
			{
				base::console::disable();
			}
		}

		return 0;
	}

	int jass_console_get(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);

		if (strcmp("enable", name) == 0)
		{
			lua_pushboolean(L, runtime::enable_console);
			return 1;
		}

		return 0;
	}

	int jass_console_read(lua_State* L)
	{
		base::console::read_post();
	
		base::console::read_req_t* req = 0;
		if (base::console::read_try(req))
		{
			if (req->overlapped.Internal == 0)
			{
				if (lua_isfunction(L, 1))
				{
					std::string temp_string = w2u(std::wstring_view(req->buffer, req->overlapped.InternalHigh), conv_method::skip);
					lua_pushvalue(L, 1);
					lua_pushlstring(L, temp_string.c_str(), temp_string.size());
					safe_call(L, 1, 0, true);
				}
			}
			base::console::read_release(req);
		}
		return 0;
	}

	void jass_console_write_string(const char* str, size_t len)
	{
		std::wstring temp_string = u2w(std::string_view(str, len), conv_method::skip);
		DWORD written_size;
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), temp_string.c_str(), temp_string.size(), &written_size, 0);
	}

	int jass_console_write(lua_State* L)
	{
		int n = lua_gettop(L);
		int i;
		lua_getglobal(L, "tostring");
		for (i = 1; i <= n; i++) {
			const char *s;
			size_t l;
			lua_pushvalue(L, -1);
			lua_pushvalue(L, i);
			lua_call(L, 1, 1);
			s = lua_tolstring(L, -1, &l);
			if (s == NULL)
				return luaL_error(L, LUA_QL("tostring") " must return a string to " LUA_QL("print"));
			if (i>1) jass_console_write_string("\t", 1);
			jass_console_write_string(s, l);
			lua_pop(L, 1);
		}
		jass_console_write_string("\n", 1);
		return 0;
	}

	int open(lua_State* L)
	{
		lua_newtable(L);
		{
			lua_newtable(L);
			{
				lua_pushstring(L, "__index");
				lua_pushcclosure(L, jass_console_get, 0);
				lua_rawset(L, -3);
	
				lua_pushstring(L, "__newindex");
				lua_pushcclosure(L, jass_console_set, 0);
				lua_rawset(L, -3);
			}
			lua_setmetatable(L, -2);
	
			lua_pushstring(L, "read");
			lua_pushcclosure(L, jass_console_read, 0);
			lua_rawset(L, -3);

			lua_pushstring(L, "write");
			lua_pushcclosure(L, jass_console_write, 0);
			lua_rawset(L, -3);
		}
		return 1;
	}
}}}}
