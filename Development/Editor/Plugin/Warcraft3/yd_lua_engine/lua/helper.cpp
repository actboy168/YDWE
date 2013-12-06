#include "../lua/helper.h"
#include "../misc/storm.h"
#include <cstring>
#include <base/util/singleton.h>
#include <base/warcraft3/jass.h>

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

	lua_State* luaL_newstate2()
	{
		unsigned int hi = jass::call("GetRandomInt", 0, 0xFFFF);
		unsigned int lo = jass::call("GetRandomInt", 0, 0xFFFF);
		lua_State *L = lua_newstate2(l_alloc, NULL, (hi << 16) | lo);
		if (L) lua_atpanic(L, &panic);
		return L;
	}

	const char* findtable(lua::state* ls, int idx, const char *fname, int szhint) 
	{
		const char *e;
		if (idx) 
			ls->pushvalue(idx);
		do 
		{
 			e = strchr(fname, '.');
			if (e == NULL) e = fname + strlen(fname);
			ls->pushlstring(fname, e - fname);
			ls->rawget(-2);
			if (ls->isnil(-1))  /* no such field? */
			{ 
				ls->pop(1);  /* remove this nil */
				ls->createtable(0, (*e == '.' ? 1 : szhint)); /* new table for field */
				ls->pushlstring(fname, e - fname);
				ls->pushvalue(-2);
				ls->settable(-4);  /* set new table into field */
			}
			else if (!ls->istable(-1))   /* field has a non-table value? */
			{
				ls->pop(2);  /* remove table and value */
				return fname;  /* return problematic part of the name */
			}
			ls->remove(-2);  /* remove previous table */
			fname = e + 1;
		} while (*e == '.');
		return NULL;
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
		findtable(ls, LUA_REGISTRYINDEX, "_LOADED", 1); 
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
		findtable(ls, LUA_REGISTRYINDEX, "_LOADED", 1); 
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
