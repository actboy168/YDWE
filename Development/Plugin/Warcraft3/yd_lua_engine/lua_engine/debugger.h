#pragma once

struct lua_State;

namespace base { namespace warcraft3 { namespace lua_engine {
	class debugger;
	debugger* debugger_create();
	void      debugger_close(debugger* dbg);
	void      debugger_attach_lua(debugger* dbg, lua_State* L);
	void      debugger_detach_lua(debugger* dbg, lua_State* L);
}}}
