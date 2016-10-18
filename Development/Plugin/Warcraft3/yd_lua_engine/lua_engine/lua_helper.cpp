#include "lua_helper.h"
#include "storm.h"
#include <cstring>	   
#include <base/util/unicode.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/war3_searcher.h>


namespace base { namespace warcraft3 { namespace lua_engine {
	namespace package {
		int searcher_storm(lua_State *L);
		int searcher_file(lua_State *L);
		int searcher_dll(lua_State *L);
	}

	static void* l_alloc(void* /*ud*/, void* ptr, size_t /*osize*/, size_t nsize) 
	{
		if (nsize == 0) 
		{
			free(ptr);
			return NULL;
		}
		else
		{
			return realloc(ptr, nsize);
		}
	}

	static int panic(lua_State *L) 
	{
		lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n", lua_tostring(L, -1));
		return 0;
	}

	uintptr_t get_random_seed()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = s.search_string("SetRandomSeed");
		ptr = *(uintptr_t*)(ptr + 0x05);
		ptr = next_opcode(ptr, 0x8B, 6);
		ptr = *(uintptr_t*)(ptr + 2);

		return *(uintptr_t*)(*(uintptr_t*)(ptr) + 4);
	}

	lua_State* luaL_newstate2()
	{
		lua_State *L = lua_newstate2(l_alloc, NULL, get_random_seed());
		if (L) lua_atpanic(L, &panic);
		return L;
	}

	int __cdecl searcher_preload(lua_State* L) 
	{
		const char *name = luaL_checkstring(L, 1);
		lua_getfield(L, LUA_REGISTRYINDEX, "_PRELOAD");
		lua_getfield(L, -1, name);
		if (lua_isnil(L, -1))  /* not found? */
			lua_pushfstring(L, "\n\tno field package.preload['%s']", name);
		return 1;
	}

	bool clear_searchers_table(lua_State* L)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
		lua_getfield(L, -1, LUA_LOADLIBNAME);
		if (lua_istable(L, -1))
		{
			lua_createtable(L, 2, 0);
			lua_pushvalue(L, -2);
			lua_pushcclosure(L, searcher_preload, 1);
			lua_rawseti(L, -2, 1);
			lua_setfield(L, -2, "searchers");
			lua_pop(L, 2);
			return true;
		}
		lua_pop(L, 2);
		return false;
	}

	bool insert_searchers_table(lua_State* L)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
		lua_getfield(L, -1, LUA_LOADLIBNAME);
		if (!lua_istable(L, -1))
		{
			lua_pop(L, 2);
			return false;
		}
		lua_pushstring(L, "?.lua");
		lua_setfield(L, -2, "path");
		lua_pushstring(L, "?.dll");
		lua_setfield(L, -2, "cpath");
		lua_getfield(L, -1, "searchers");
		if (!lua_istable(L, -1))
		{
			lua_pop(L, 3);
			return false;
		}
		for (int i = 1;; i++)
		{
			lua_rawgeti(L, -1, i);
			if (lua_isnil(L, -1))
			{
				lua_pushvalue(L, -3);
				lua_pushcclosure(L, package::searcher_file, 1);
				lua_rawseti(L, -3, i);
				lua_pushvalue(L, -3);
				lua_pushcclosure(L, package::searcher_storm, 1);
				lua_rawseti(L, -3, i + 1);
				lua_pushvalue(L, -3);
				lua_pushcclosure(L, package::searcher_dll, 1);
				lua_rawseti(L, -3, i + 2);
				lua_pop(L, 4);
				return true;
			}
			lua_pop(L, 1);
		}
	}

}}}
