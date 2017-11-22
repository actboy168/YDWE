#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)
#include <Windows.h>
#include <base/warcraft3/war3_searcher.h>

namespace WideScreen
{
	void initialize(HWND War3Wnd);
}

static int version(lua_State* L) {
	lua_pushinteger(L, base::warcraft3::get_war3_searcher().get_version());
	return 1;
}

static int support_widescreen(lua_State* L) {
	WideScreen::initialize(HWND(luaL_checkinteger(L, 1)));
	return 0;
}

extern "C" __declspec(dllexport)
int luaopen_war3patch(lua_State* L)
{
	static luaL_Reg api[] = {
		{ "version", version },
		{ "support_widescreen", support_widescreen },
		{ NULL, NULL }
	};
	luaL_newlib(L, api);
	return 1;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}
