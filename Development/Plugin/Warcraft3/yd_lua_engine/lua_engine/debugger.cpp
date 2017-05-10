#include "debugger.h"
#include <Windows.h>
#include <lua.hpp>

namespace base { namespace warcraft3 { namespace lua_engine { namespace debugger {
	int open(lua_State* L)
	{
		HMODULE m = LoadLibraryW(L"debugger.dll");
		if (!m) {
			return 0;
		}
		lua_CFunction openf = (lua_CFunction)GetProcAddress(m, "luaopen_debugger");
		if (!openf) {
			return 0;
		}
		return openf(L);
	}
}}}}
