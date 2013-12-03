#pragma once

#include <base/lua/state.h>
#include <base/warcraft3/jass/func_value.h>
#include <lua.hpp>
#include <cstdint>

namespace base { namespace warcraft3 { namespace lua_engine {

	int                 safe_pcall (lua_State* pState, int nargs, int nresults);
	uint32_t __fastcall jass_callback(uint32_t param);
	uint32_t            cfunction_to_code(lua::state* ls, uint32_t index);

	struct callback
	{
	public:
		callback();
		callback(lua::state* ls, uint32_t index);
		callback(uint32_t ref);
		bool      call_pre(lua::state* ls) const;
		uintptr_t call(lua::state* ls, size_t param_size, jass::variable_type result_vt) const;

	private:
		uint32_t ref_;
	};


}}}
