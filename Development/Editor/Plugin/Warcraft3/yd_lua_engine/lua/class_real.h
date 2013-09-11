#pragma once

#include "../lua/helper.h"
#include <base/warcraft3/jass.h>

_BASE_BEGIN
namespace warcraft3 { namespace lua_engine {

	template <class T>
	T    jreal_read(lua_State* L, int index);

	void jreal_push(lua_State* L, float f);
	void jreal_push(lua_State* L, jass::jreal_t f);

}}
_BASE_END
