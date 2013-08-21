#pragma once

#include <ydwe/lua/state.h>

namespace ydwe { namespace warcraft3 { namespace lua_engine { namespace lua {

	using namespace ydwe::lua;

	bool replace_searchers_table(lua::state* ls);
	bool insert_searchers_table(lua::state* ls);
}}}}
