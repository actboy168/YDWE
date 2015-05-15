#pragma once

#include <lua.hpp>

namespace base { namespace warcraft3 { namespace lua_engine {

	lua_State* luaL_newstate2();
	bool clear_searchers_table(lua_State* L);
	bool insert_searchers_table(lua_State* L);
}}}
