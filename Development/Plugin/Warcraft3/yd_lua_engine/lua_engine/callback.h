#pragma once

#include <base/warcraft3/jass/func_value.h>
#include <lua.hpp>
#include <cstdint>

namespace base { namespace warcraft3 { namespace lua_engine {

	int                 safe_call_not_sleep(lua_State* L, int nargs, int nresults, bool err_func);
	int                 safe_call(lua_State* L, int nargs, int nresults, bool err_func);
	uintptr_t           safe_call_ref(lua_State* L, uint32_t ref, size_t nargs, jass::variable_type result_vt);
	uint32_t            cfunction_to_code(lua_State* L, uint32_t index);
}}}
