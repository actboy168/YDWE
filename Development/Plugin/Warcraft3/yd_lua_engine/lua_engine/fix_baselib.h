#pragma once

#include <lua.hpp>

namespace base { namespace warcraft3 { namespace lua_engine {
	lua_State* newstate();
	int fix_baselib(lua_State* L);
}}}
