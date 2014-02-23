#pragma once

#include <base/config.h>
#include <lua.hpp>

namespace base { namespace lua {
	_BASE_API bool allow_yield(lua_State* L);
}}
