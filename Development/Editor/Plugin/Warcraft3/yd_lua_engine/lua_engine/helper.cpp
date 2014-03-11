#include "helper.h"
#include "storm.h"
#include <cstring>
#include <base/util/singleton.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/war3_searcher.h>

namespace base { namespace warcraft3 { namespace lua_engine {

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
		luai_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n", lua_tostring(L, -1));
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

	int __cdecl searcher_preload(lua::state* ls) 
	{
		const char *name = luaL_checkstring(ls->self(), 1);
		ls->getfield(LUA_REGISTRYINDEX, "_PRELOAD");
		ls->getfield(-1, name);
		if (ls->isnil(-1))  /* not found? */
			lua_pushfstring(ls->self(), "\n\tno field package.preload['%s']", name);
		return 1;
	}

	int __cdecl searcher_storm(lua::state* ls) 
	{
		const char* name   = luaL_checkstring(ls->self(), 1);
		const char* buffer = nullptr;
		size_t      size   = 0;

		storm& s = util::singleton_nonthreadsafe<storm>::instance();
		if (s.load_file(name, (const void**)&buffer, &size))
		{
			int stat = (luaL_loadbuffer(ls->self(), buffer, size, name) == LUA_OK);
			s.unload_file(buffer);

			if (stat) 
			{
				ls->pushstring(name);
				return 2;
			}
			else
			{
				return luaL_error(ls->self(), "error loading module " 
					LUA_QS " from file " 
					LUA_QS ":\n\t%s", 
					ls->tostring(1), name, ls->tostring(-1));
			}
		}

		return 1;
	}

	bool clear_searchers_table(lua::state* ls)
	{
		ls->getfield(LUA_REGISTRYINDEX, "_LOADED"); 
		ls->getfield(-1, LUA_LOADLIBNAME);

		if (ls->istable(-1)) 
		{
			ls->createtable(2, 0);

			ls->pushvalue(-2);
			ls->pushcclosure(searcher_preload, 1);
			ls->rawseti(-2, 1);

			ls->setfield(-2, "searchers");

			return true;
		}

		return false;
	}

	bool insert_searchers_table(lua::state* ls)
	{
		ls->getfield(LUA_REGISTRYINDEX, "_LOADED"); 
		ls->getfield(-1, LUA_LOADLIBNAME);

		if (ls->istable(-1)) 
		{
			ls->getfield(-1, "searchers");

			if (ls->istable(-1)) 
			{
				lua_pushliteral(ls->self(), "");

				for (int i = 1; ; i++) 
				{
					ls->rawgeti(-2, i);
					if (ls->isnil(-1)) 
					{
						ls->pushcclosure(searcher_storm, 0);
						ls->rawseti(-4, i);
						return true;
					}
					ls->pop(1);
				}
			}
		}

		return false;
	}

}}}
