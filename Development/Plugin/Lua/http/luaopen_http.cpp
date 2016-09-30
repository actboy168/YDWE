#include <lua.hpp>	  
#include "http_thread.h"

http::thread http_thd;

static int http_open(lua_State* L)
{
	bool        done = false;
	int         res_code = 0;
	std::string res_msg = "";

	size_t urlsize = 0;
	const char* url = lua_tolstring(L, 1, &urlsize);
	if (lua_gettop(L) >= 2)	{
		size_t bodysize = 0;
		const char* body = lua_tolstring(L, 2, &bodysize);
		http_thd.post(std::string(url, urlsize), std::string(body, bodysize))
		.then([&](int code, const std::string& msg){
			res_code = code;
			res_msg = msg;
			done = true;
		});
	}
	else {
		http_thd.get(std::string(url, urlsize))
		.then([&](int code, const std::string& msg){
			res_code = code;
			res_msg = msg;
			done = true;
		});
	}
	while (!done) {
		http_thd.run_one();
	}
	if (res_code != 0) {
		lua_pushnil(L);
		lua_pushinteger(L, res_code);
		return 2;
	}
	lua_pushlstring(L, res_msg.c_str(), res_msg.size());
	return 1;
}

extern "C" __declspec(dllexport) int luaopen_http(lua_State* L);

int luaopen_http(lua_State* L)
{
	static luaL_Reg func[] = {
		{ "open", http_open },
		{ NULL, NULL }
	};
	luaL_newlibtable(L, func);
	luaL_setfuncs(L, func, 0);
	lua_setglobal(L, "http");
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
