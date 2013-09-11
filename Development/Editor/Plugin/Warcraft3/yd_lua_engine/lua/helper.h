#pragma once

#include <base/lua/state.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	bool clear_searchers_table(lua::state* ls);
	bool insert_searchers_table(lua::state* ls);
}}}
