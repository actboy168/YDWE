#include "logging.h"
#include "logging_backend.h" 
#include <map>
#include <lua.hpp>

namespace logging
{
	static int LOGGING;

	lua_State* lastL = nullptr;

	void set_manager(lua_State* L, manager* mgr)
	{
		lua_pushlightuserdata(L, mgr);
		lua_rawsetp(L, LUA_REGISTRYINDEX, &LOGGING);
		lastL = L;
	}

	manager* get_manager(lua_State* L)
	{
		if (LUA_TLIGHTUSERDATA != lua_rawgetp(L, LUA_REGISTRYINDEX, &LOGGING)) {
			lua_pop(L, 1);
			return nullptr;
		}
		manager* res = static_cast<manager*>(lua_touserdata(L, -1));
		lua_pop(L, 1);
		return res;
	}

	logger* get_logger(lua_State* L, const char* name)
	{
		manager* mgr = get_manager(L);
		if (mgr) {
			return mgr->get_logger(name);
		}
		return nullptr;
	}

	logger* get_logger(const char* name)
	{
		if (!lastL) {
			return nullptr;
		}
		return get_logger(lastL, name);
	}
}
