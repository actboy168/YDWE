#pragma once

#include "lua_helper.h"
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace lua_engine {
	void jarray_make_mt(lua_State* L);
	int  jarray_create(lua_State* L, uintptr_t value);
}}}
