#include "logging.h"
#include "logging_backend.h" 
#include <map>
#include <lua.hpp>

namespace logging
{
	static int LOGGING;

	static int logger_gc(lua_State* L) {
		logger* lg = (logger*)lua_touserdata(L, -1);
		lg->~logger();
		return 0;
	}

	logger* create(lua_State* L, const fs::path& root, const std::wstring& name) {
		logger* lg = (logger*)lua_newuserdata(L, sizeof logger);
		new (lg) logger(backend(root, name));
		if (luaL_newmetatable(L, "ydwe::logger")) {
			lua_pushcfunction(L, logger_gc);
			lua_setfield(L, -2, "__gc");
		}
		lua_setmetatable(L, -2);
		lua_rawsetp(L, LUA_REGISTRYINDEX, &LOGGING);
		return lg;
	}

	logger* get(lua_State* L) {
		if (LUA_TUSERDATA != lua_rawgetp(L, LUA_REGISTRYINDEX, &LOGGING)) {
			lua_pop(L, 1);
			return nullptr;
		}
		logger* lg = (logger*)lua_touserdata(L, -1);
		lua_pop(L, 1);
		return lg;
	}
}
