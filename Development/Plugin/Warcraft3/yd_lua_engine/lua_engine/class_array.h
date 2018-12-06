#pragma once

#include <lua.hpp>
#include <warcraft3/jass.h>

namespace warcraft3::lua_engine {
	void jarray_make_mt(lua_State* L);
	int  jarray_create(lua_State* L, uintptr_t value);
}
