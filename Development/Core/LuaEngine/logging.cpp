#include "logging.h"
#include "logging_backend.h" 
#include <map>
#include <lua.hpp>

namespace logging
{
	static int LOGGING;

	lua_State* lastL = nullptr;

	void set_logger(lua_State* L, logger* lg)
	{
		lua_pushlightuserdata(L, lg);
		lua_rawsetp(L, LUA_REGISTRYINDEX, &LOGGING);
		lastL = L;
	}

	logger* get_logger(lua_State* L)
	{
		if (LUA_TLIGHTUSERDATA != lua_rawgetp(L, LUA_REGISTRYINDEX, &LOGGING)) {
			lua_pop(L, 1);
			return nullptr;
		}
		logger* res = static_cast<logger*>(lua_touserdata(L, -1));
		lua_pop(L, 1);
		return res;
	}

	logger* get_logger()
	{
		if (!lastL) {
			return nullptr;
		}
		return get_logger(lastL);
	}
}
