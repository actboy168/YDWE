#include <lua.hpp>	  
#include <luabind/luabind.hpp>
#include "http_thread.h"

http::thread http_thd;

static void http_open(lua_State* L, const std::string& url)
{
	bool        done = false;
	int         res_code = 0;
	std::string res_msg = "";

	http_thd.request(url).then([&](int code, const std::string& msg){
		res_code = code;
		res_msg = msg;
		done = true;
	});

	while (!done) {
		http_thd.run_one();
	}

	if (res_code != 0)
	{
		lua_pushnil(L);
		lua_pushinteger(L, res_code);
	}
	else 
	{
		lua_pushlstring(L, res_msg.c_str(), res_msg.size());
	}
}

extern "C" __declspec(dllexport) int luaopen_http(lua_State* L);

int luaopen_http(lua_State *pState)
{
	using namespace luabind;

	module(pState, "http")
	[
		def("open",    &http_open)
	];

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
