#include "handle.h"
#include "runtime.h"
#include "../lua/jassbind.h"
#include <base/warcraft3/jass.h>
#include <base/warcraft3/hashtable.h>
#include <base/hook/fp_call.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	void handle_set_ref(jass::jhandle_t h, bool dec)
	{
		uintptr_t vm = get_jass_virtual_machine();
		base::fast_call<void>(*(uintptr_t*)(vm + 0x28A0), h, dec ? 1 : 0, *(uintptr_t*)(vm + 0x28A4));
	}

	void handle_add_ref(jass::jhandle_t h)
	{
		handle_set_ref(h, false);
	}

	void handle_release(jass::jhandle_t h)
	{
		handle_set_ref(h, true);
	}

#define LUA_JASS_HANDLE "jhandle_t"

	int handle_tostring(lua_State *L, jass::jhandle_t h)
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

	void handle_ud_push(lua::state* ls, jass::jhandle_t value)
	{
		if (!value)
		{
			return ls->pushnil();
		}

		jass::jhandle_t* hptr = (jass::jhandle_t*)ls->newuserdata(sizeof(jass::jhandle_t));
		*hptr = value;
		handle_add_ref(value);
		luaL_setmetatable(ls->self(), LUA_JASS_HANDLE);
		return;
	}

	jass::jhandle_t handle_ud_read(lua::state* ls, int index)
	{
		if (ls->isnil(index))
		{
			return 0; 
		}
		
		jass::jhandle_t* hptr = ls->checkudata<jass::jhandle_t*>(index, LUA_JASS_HANDLE);
		return *hptr;
	}

	int handle_ud_eq(lua_State *L)
	{
		lua::state* ls = (lua::state*)L;
		jass::jhandle_t a = (jass::jhandle_t)handle_ud_read(ls, 1);
		jass::jhandle_t b = (jass::jhandle_t)handle_ud_read(ls, 2);
		ls->pushboolean(a == b);
		return 1;
	}

	int handle_ud_tostring(lua_State *L)
	{
		return handle_tostring(L, (jass::jhandle_t)handle_ud_read((lua::state*)L, 1));
	}

	int handle_ud_gc(lua_State *L)
	{
		lua::state* ls = (lua::state*)L;
		jass::jhandle_t h = (jass::jhandle_t)handle_ud_read(ls, 1);
		handle_release(h);
		return 0;
	}

	void handle_ud_make_mt(lua::state* ls)
	{
		luaL_Reg lib[] = {
			{ "__eq",       handle_ud_eq },
			{ "__tostring", handle_ud_tostring },
			{ "__gc",       handle_ud_gc },
			{ NULL, NULL },
		};

		luaL_newmetatable(ls->self(), LUA_JASS_HANDLE);
#if LUA_VERSION_NUM >= 502
		luaL_setfuncs(ls->self(), lib, 0);
		ls->pop(1);
#else
		luaL_register(ls->self(), NULL, lib);
#endif
	}

	void handle_lud_push(lua::state* ls, jass::jhandle_t value)
	{
		if (!value)
		{
			return ls->pushnil();
		}

		return ls->pushlightuserdata((void*)value);
	}

	jass::jhandle_t handle_lud_read(lua::state* ls, int index)
	{
		return (jass::jhandle_t)ls->touserdata(index);
	}

	int handle_lud_eq(lua_State *L)
	{
		lua::state* ls = (lua::state*)L;
		jass::jhandle_t a = (jass::jhandle_t)handle_lud_read(ls, 1);
		jass::jhandle_t b = (jass::jhandle_t)handle_lud_read(ls, 2);
		ls->pushboolean(a == b);
		return 1;
	}

	int handle_lud_tostring(lua_State *L)
	{
		return handle_tostring(L, (jass::jhandle_t)handle_lud_read((lua::state*)L, 1));
	}

	void handle_lud_make_mt(lua::state* ls)
	{
		luaL_Reg lib[] = {
			{ "__eq",       handle_lud_eq },
			{ "__tostring", handle_lud_tostring },
			{ NULL, NULL },
		};
	
		ls->pushlightuserdata(NULL);
		luaL_newlib(ls->self(), lib);
		ls->setmetatable(-2);
		ls->pop(1);
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
			return handle_ud_read(this, index);
		}
		else
		{
			// lightuserdata
			return handle_lud_read(this, index);
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
			return handle_ud_push(this, value);
		}
		else
		{
			// lightuserdata
			return handle_lud_push(this, value);
		}
	}
}}}
