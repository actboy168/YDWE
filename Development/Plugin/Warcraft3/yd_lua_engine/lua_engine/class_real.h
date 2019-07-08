#pragma once

#include <lua.hpp>
#include <warcraft3/jass.h>

namespace warcraft3::lua_engine::jreal {
	template <class T>
	T    read(lua_State* L, int index);
	void push_precise(lua_State* L, jass::jreal_t f);
	void init(lua_State* L);
}
