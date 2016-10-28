#pragma once

struct lua_State;

namespace base { namespace warcraft3 { namespace lua_engine {
	class debugger;
	debugger* debugger_create(lua_State* L);
	void      debugger_close(debugger* dbg);
}}}
