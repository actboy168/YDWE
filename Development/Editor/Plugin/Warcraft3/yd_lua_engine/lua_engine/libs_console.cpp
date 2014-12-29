#include <base/lua/state.h>
#include <base/util/console.h>
#include <base/util/unicode.h> 
#include "callback.h"

namespace base {  namespace warcraft3 { namespace lua_engine {

	namespace runtime
	{
		extern bool enable_console;
	}

	int jass_console_set(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;
		const char* name = ls->tostring(2);

		if (strcmp("enable", name) == 0)
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

		return 0;
	}

	int jass_console_get(lua_State* L)
	{
		lua::state* ls = (lua::state*)L;
		const char* name = ls->tostring(2);

		if (strcmp("enable", name) == 0)
		{
			ls->pushboolean(runtime::enable_console);
			return 1;
		}

		return 0;
	}

	int jass_console_read(lua::state* ls)
	{
		console::read_post();
	
		console::read_req_t* req = 0;
		if (console::read_try(req))
		{
			if (req->overlapped.Internal == 0)
			{
				if (ls->isfunction(1))
				{
					std::string temp_string = util::w2u(std::wstring_view(req->buffer, req->overlapped.InternalHigh), util::conv_method::skip);
					ls->pushvalue(1);
					ls->pushlstring(temp_string.c_str(), temp_string.size());
					safe_call(ls, 1, 0, true);
				}
			}
			console::read_release(req);
		}
		return 0;
	}

	void jass_console_write_string(const char* str, size_t len)
	{
		std::wstring temp_string = util::u2w(std::string_view(str, len), util::conv_method::skip);
		DWORD written_size;
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), temp_string.c_str(), temp_string.size(), &written_size, 0);
	}

	int jass_console_write(lua::state* ls)
	{
		int n = ls->gettop();
		int i;
		ls->getglobal("tostring");
		for (i = 1; i <= n; i++) {
			const char *s;
			size_t l;
			ls->pushvalue(-1);
			ls->pushvalue(i);
			ls->call(1, 1);
			s = ls->tolstring(-1, &l);
			if (s == NULL)
				return luaL_error(ls->self(), LUA_QL("tostring") " must return a string to " LUA_QL("print"));
			if (i>1) jass_console_write_string("\t", 1);
			jass_console_write_string(s, l);
			ls->pop(1);
		}
		jass_console_write_string("\n", 1);
		return 0;
	}

	int jass_console(lua::state* ls)
	{
		ls->newtable();
		{
			ls->newtable();
			{
				ls->pushstring("__index");
				ls->pushcclosure((lua::cfunction)jass_console_get, 0);
				ls->rawset(-3);
	
				ls->pushstring("__newindex");
				ls->pushcclosure((lua::cfunction)jass_console_set, 0);
				ls->rawset(-3);
			}
			ls->setmetatable(-2);
	
			ls->pushstring("read");
			ls->pushcclosure(jass_console_read, 0);
			ls->rawset(-3);

			ls->pushstring("write");
			ls->pushcclosure(jass_console_write, 0);
			ls->rawset(-3);
		}
		return 1;
	}
}}}
