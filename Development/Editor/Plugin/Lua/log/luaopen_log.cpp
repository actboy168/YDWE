#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)

#include <windows.h>
#include "logging.h"

logging::logger lg;

int llog_print(lua_State *L)
{
	boost::log::BOOST_LOG_VERSION_NAMESPACE::trivial::severity_level lv = (boost::log::BOOST_LOG_VERSION_NAMESPACE::trivial::severity_level)lua_tointeger(L, lua_upvalueindex(1));
	int n = lua_gettop(L);

	luaL_Buffer b;
	luaL_buffinit(L, &b);
	lua_getglobal(L, "tostring"); 
	for (int i = 1; i <= n; i++)
	{
		const char *s;
		size_t l;
		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tolstring(L, -1, &l);
		if (s == NULL)
			return luaL_error(L, "'tostring' must return a string to 'print'");
		if (i>1) luaL_addchar(&b, '\t');
		luaL_addlstring(&b, s, l);
		lua_pop(L, 1);
	}
	luaL_pushresult(&b);
	size_t l;
	const char *s = lua_tolstring(L, -1, &l);
	BOOST_LOG_SEV(lg, lv) << std::string(s, l);
	return 0;
}

extern "C" __declspec(dllexport) int luaopen_log(lua_State* L);

int luaopen_log(lua_State* L)
{
	lg = logging::get_logger("lua");

	struct llog_Reg {
		const char *name;
		lua_Integer upvalue;
	};

	static llog_Reg func[] = {
		{ "trace", boost::log::trivial::trace },
		{ "debug", boost::log::trivial::debug },
		{ "info",  boost::log::trivial::info },
		{ "warn",  boost::log::trivial::warning },
		{ "error", boost::log::trivial::error },
		{ "fatal", boost::log::trivial::fatal },
		{ NULL, NULL }
	};

	luaL_newlibtable(L, func);
	for (const llog_Reg* l = func; l->name != NULL; l++)
	{
		lua_pushinteger(L, l->upvalue);
		lua_pushcclosure(L, llog_print, 1);
		lua_setfield(L, -2, l->name);
	}

	lua_setglobal(L, "log");
	return 0;
}

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}
