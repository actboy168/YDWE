#pragma once

#include <lua.hpp>
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace lua_engine { namespace jreal {
	template <class T>
	T    read(lua_State* L, int index);
	void push_precise(lua_State* L, jass::jreal_t f);
	void init(lua_State* L);
}}}}
