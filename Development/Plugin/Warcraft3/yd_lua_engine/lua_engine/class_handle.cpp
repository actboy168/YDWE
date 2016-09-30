#include "class_handle.h"
#include "libs_runtime.h"
#include "jassbind.h"
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

	void jhandle_ud_push(lua_State* L, jass::jhandle_t value)
	{
		if (!value)
		{
			return lua_pushnil(L);
		}

		runtime::handle_ud_get_table(L);
		lua_pushinteger(L, value);
		lua_rawget(L, -2);
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			jass::jhandle_t* hptr = (jass::jhandle_t*)lua_newuserdata(L, sizeof(jass::jhandle_t));
			*hptr = value;
			jass::handle_ref(value);
			luaL_setmetatable(L, LUA_JASS_HANDLE);

			lua_pushinteger(L, value);
			lua_pushvalue(L, -2);
			lua_rawset(L, -4);
		}

		lua_remove(L, -2);
		return;
	}

	jass::jhandle_t jhandle_ud_read(lua_State* L, int index)
	{
		if (lua_isnil(L, index))
		{
			return 0; 
		}
		
		jass::jhandle_t* hptr = (jass::jhandle_t*)luaL_checkudata(L, index, LUA_JASS_HANDLE);
		return *hptr;
	}

	int jhandle_ud_eq(lua_State *L)
	{
		jass::jhandle_t a = jhandle_ud_read(L, 1);
		jass::jhandle_t b = jhandle_ud_read(L, 2);
		lua_pushboolean(L, a == b);
		return 1;
	}

	int jhandle_ud_tostring(lua_State *L)
	{
		return jhandle_tostring(L, jhandle_ud_read(L, 1));
	}

	int jhandle_ud_gc(lua_State *L)
	{
		jass::jhandle_t h = jhandle_ud_read(L, 1);
		runtime::handle_ud_get_table(L);
		lua_pushinteger(L, h);
		lua_pushnil(L);
		lua_rawset(L, -3);

		jass::handle_unref(h);
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

	void jhandle_lud_push(lua_State* L, jass::jhandle_t value)
	{
		if (!value)
		{
			return lua_pushnil(L);
		}

		return lua_pushlightuserdata(L, (void*)value);
	}

	jass::jhandle_t jhandle_lud_read(lua_State* L, int index)
	{
		return (jass::jhandle_t)lua_touserdata(L, index);
	}

	int jhandle_lud_eq(lua_State *L)
	{
		jass::jhandle_t a = jhandle_lud_read(L, 1);
		jass::jhandle_t b = jhandle_lud_read(L, 2);
		lua_pushboolean(L, a == b);
		return 1;
	}

	int jhandle_lud_tostring(lua_State *L)
	{
		return jhandle_tostring(L, jhandle_lud_read(L, 1));
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
	
	namespace jassbind
	{
		jass::jhandle_t read_handle(lua_State* L, int index)
		{
			if (0 == runtime::handle_level)
			{
				// unsigned
				return (jass::jhandle_t)lua_tointeger(L, index);
			}
			else if (2 == runtime::handle_level)
			{
				// userdata
				return jhandle_ud_read(L, index);
			}
			else
			{
				// lightuserdata
				return jhandle_lud_read(L, index);
			}
		}

		void push_handle(lua_State* L, jass::jhandle_t value)
		{
			if (0 == runtime::handle_level)
			{
				// unsigned
				return lua_pushinteger(L, value);
			}
			else if (2 == runtime::handle_level)
			{
				// userdata
				return jhandle_ud_push(L, value);
			}
			else
			{
				// lightuserdata
				return jhandle_lud_push(L, value);
			}
		}
	}
}}}
