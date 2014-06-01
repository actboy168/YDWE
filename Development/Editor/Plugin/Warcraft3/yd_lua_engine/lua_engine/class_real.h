#pragma once

#include "lua_helper.h"
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	template <class T>
	T    jreal_read(lua_State* L, int index);

	void jreal_push(lua_State* L, float f);
	void jreal_push(lua_State* L, jass::jreal_t f);
	void jreal_make_mt(lua_State* L);
}}}
