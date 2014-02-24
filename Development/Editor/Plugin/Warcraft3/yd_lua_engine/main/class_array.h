#pragma once

#include "../lua/helper.h"
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace lua_engine {
	void array_make_mt(lua_State* L);
	int array_create(lua_State* L, uintptr_t value);
}}}
