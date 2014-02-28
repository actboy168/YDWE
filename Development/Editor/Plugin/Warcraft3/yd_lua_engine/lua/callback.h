#pragma once

#include <base/lua/state.h>
#include <base/warcraft3/jass/func_value.h>
#include <lua.hpp>
#include <cstdint>

namespace base { namespace warcraft3 { namespace lua_engine {

	int                 safe_call(lua::state* ls, int nargs, int nresults, int error_handle);
	uintptr_t           safe_call_ref(lua::state* ls, uint32_t ref, size_t nargs, jass::variable_type result_vt);
	uint32_t            cfunction_to_code(lua::state* ls, uint32_t index);
}}}
