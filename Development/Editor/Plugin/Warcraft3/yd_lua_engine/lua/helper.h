#pragma once

#include <base/lua/state.h>

_BASE_BEGIN
namespace warcraft3 { namespace lua_engine {

	bool clear_searchers_table(lua::state* ls);
	bool insert_searchers_table(lua::state* ls);
}}

_BASE_END
