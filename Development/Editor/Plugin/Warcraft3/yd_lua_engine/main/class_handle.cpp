#include "class_handle.h"
#include "libs_runtime.h"
#include "../lua/jassbind.h"
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	int jhandle_tostring(lua_State *L, jass::jhandle_t h)
	{
		static char hex[] = "0123456789ABCDEF";

		luaL_Buffer b;
		luaL_buffinitsize(L, &b, 28);
		luaL_addstring(&b, "handle: 0x");

		bool strip = true;
		for (int i = 7; i >= 0; i--)
		{
			int c = (h >> (i * 4)) & 0xF;
			if (strip && c == 0)
			{
				continue;
			}
			strip = false;
			luaL_addchar(&b, hex[c]);
		}

		if (strip)
		{
			luaL_addchar(&b, '0');
		}

		luaL_pushresult(&b);

		return 1;
	}

#define LUA_JASS_HANDLE "jhandle_t"

	void jhandle_ud_push(lua::state* ls, jass::jhandle_t value)
	{
		if (!value)
		{
			return ls->pushnil();
		}

		runtime::handle_ud_get_table(ls);
		ls->pushunsigned(value);
		ls->rawget(-2);
		if (ls->isnil(-1))
		{
			ls->pop(1);
			jass::jhandle_t* hptr = (jass::jhandle_t*)ls->newuserdata(sizeof(jass::jhandle_t));
			*hptr = value;
			jass::handle_add_ref(value);
			luaL_setmetatable(ls->self(), LUA_JASS_HANDLE);

			ls->pushunsigned(value);
			ls->pushvalue(-2);
			ls->rawset(-4);
		}

		ls->remove(-2);
		return;
	}

	jass::jhandle_t jhandle_ud_read(lua::state* ls, int index)
	{
		if (ls->isnil(index))
		{
			return 0; 
		}
		
		jass::jhandle_t* hptr = ls->checkudata<jass::jhandle_t*>(index, LUA_JASS_HANDLE);
		return *hptr;
	}

	int jhandle_ud_eq(lua_State *L)
	{
		lua::state* ls = (lua::state*)L;
		jass::jhandle_t a = jhandle_ud_read(ls, 1);
		jass::jhandle_t b = jhandle_ud_read(ls, 2);
		ls->pushboolean(a == b);
		return 1;
	}

	int jhandle_ud_tostring(lua_State *L)
	{
		return jhandle_tostring(L, jhandle_ud_read((lua::state*)L, 1));
	}

	int jhandle_ud_gc(lua_State *L)
	{
		lua::state* ls = (lua::state*)L;
		jass::jhandle_t h = jhandle_ud_read(ls, 1);
		runtime::handle_ud_get_table(ls);
		ls->pushunsigned(h);
		ls->pushnil();
		ls->rawset(-3);

		jass::handle_release(h);
		return 0;
	}

	void jhandle_ud_make_mt(lua_State *L)
	{
		luaL_Reg lib[] = {
			{ "__eq",       jhandle_ud_eq },
			{ "__tostring", jhandle_ud_tostring },
			{ "__gc",       jhandle_ud_gc },
			{ NULL, NULL },
		};

		luaL_newmetatable(L, LUA_JASS_HANDLE);
#if LUA_VERSION_NUM >= 502
		luaL_setfuncs(L, lib, 0);
		lua_pop(L, 1);
#else
		luaL_register(L, NULL, lib);
#endif
	}

	void jhandle_lud_push(lua::state* ls, jass::jhandle_t value)
	{
		if (!value)
		{
			return ls->pushnil();
		}

		return ls->pushlightuserdata((void*)value);
	}

	jass::jhandle_t jhandle_lud_read(lua::state* ls, int index)
	{
		return (jass::jhandle_t)ls->touserdata(index);
	}

	int jhandle_lud_eq(lua_State *L)
	{
		lua::state* ls = (lua::state*)L;
		jass::jhandle_t a = jhandle_lud_read(ls, 1);
		jass::jhandle_t b = jhandle_lud_read(ls, 2);
		ls->pushboolean(a == b);
		return 1;
	}

	int jhandle_lud_tostring(lua_State *L)
	{
		return jhandle_tostring(L, jhandle_lud_read((lua::state*)L, 1));
	}

	void jhandle_lud_make_mt(lua_State *L)
	{
		luaL_Reg lib[] = {
			{ "__eq",       jhandle_lud_eq },
			{ "__tostring", jhandle_lud_tostring },
			{ NULL, NULL },
		};
	
		lua_pushlightuserdata(L, NULL);
		luaL_newlib(L, lib);
		lua_setmetatable(L, -2);
		lua_pop(L, 1);
	}
	
	jass::jhandle_t jassbind::read_handle(int index)
	{
		if (0 == runtime::handle_level)
		{
			// unsigned
			return (jass::jhandle_t)mybase::tounsigned(index);
		}
		else if (2 == runtime::handle_level)
		{
			// userdata
			return jhandle_ud_read(this, index);
		}
		else
		{
			// lightuserdata
			return jhandle_lud_read(this, index);
		}
	}

	void jassbind::push_handle(jass::jhandle_t value)
	{
		if (0 == runtime::handle_level)
		{
			// unsigned
			return mybase::pushunsigned(value);
		}
		else if (2 == runtime::handle_level)
		{
			// userdata
			return jhandle_ud_push(this, value);
		}
		else
		{
			// lightuserdata
			return jhandle_lud_push(this, value);
		}
	}
}}}
