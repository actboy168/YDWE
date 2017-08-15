#include "lua_helper.h"
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
}}}
