#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)
#include <Windows.h>
#include <base/hook/patch.h>
#include <warcraft3/war3_searcher.h>
#include <warcraft3/version.h>

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

template <size_t n>
void patch_all(const char(&from)[n], const char(&to)[n])
{
	auto& s = base::warcraft3::get_war3_searcher();
	auto[frist, last] = s.text();
	for (uintptr_t ptr = frist; ptr < last - (n - 1); ++ptr)
	{
		if (0 == memcmp((const void*)ptr, from, n - 1))
		{
			base::hook::patch(ptr, to);
			ptr += (n - 2);
		}
	}
}

static int remove_sizelimit(lua_State* L) {
	if (base::warcraft3::get_war3_searcher().get_version() >= base::warcraft3::version_127a)
	{
		return 0;
	}
	else if (base::warcraft3::get_war3_searcher().get_version() >= base::warcraft3::version_124b)
	{
		patch_all("\x3D\x00\x00\x80\x00", "\x3D\xFF\xFF\xFF\xFF");
	}
	else
	{
		patch_all("\x3D\x00\x00\x40\x00", "\x3D\xFF\xFF\xFF\xFF");
	}
	return 0;
}

extern "C" __declspec(dllexport)
int luaopen_war3patch(lua_State* L)
{
	static luaL_Reg api[] = {
		{ "version", version },
		{ "support_widescreen", support_widescreen },
		{ "remove_sizelimit", remove_sizelimit },
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
