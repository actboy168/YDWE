#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)

#include <windows.h>
#include "logging.h"

#define buffonstack(B)	((B)->b != (B)->init.b)

static int llog_print(lua_State *L, logging::level lv) {
	logging::logger* lg = logging::get(L);
	int n = lua_gettop(L);

	luaL_Buffer b;
	luaL_buffinit(L, &b);
	lua_getglobal(L, "tostring");
	for (int i = 1; i <= n; i++) {
		const char *s;
		size_t l;
		lua_pushvalue(L, n + 1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tolstring(L, -1, &l);
		if (buffonstack(&b)) lua_insert(L, -2);
		if (s == NULL)
			return luaL_error(L, "'tostring' must return a string to 'print'");
		if (i>1) luaL_addchar(&b, '\t');
		luaL_addlstring(&b, s, l);
		lua_remove(L, n + 2);
	}
	luaL_pushresult(&b);
	size_t l;
	const char *s = lua_tolstring(L, -1, &l);
	LOGGING_STREAM(*lg, lv) << std::string(s, l);
	return 0;
}

static int llog_trace(lua_State *L) { return llog_print(L, logging::level::trace); }
static int llog_debug(lua_State *L) { return llog_print(L, logging::level::debug); }
static int llog_info (lua_State *L) { return llog_print(L, logging::level::info); }
static int llog_warn (lua_State *L) { return llog_print(L, logging::level::warn); }
static int llog_error(lua_State *L) { return llog_print(L, logging::level::error); }
static int llog_fatal(lua_State *L) { return llog_print(L, logging::level::fatal); }

extern "C"
#if defined(_WIN32)
__declspec(dllexport)
#endif
int luaopen_log(lua_State* L) {
	static luaL_Reg func[] = {
		{ "trace",  llog_trace },
		{ "debug",  llog_debug },
		{ "info",   llog_info },
		{ "warn",   llog_warn },
		{ "error",  llog_error },
		{ "fatal",  llog_fatal },
		{ NULL, NULL }
	};
	luaL_newlib(L, func);
	return 1;
}
