#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)
#include <base/i18n/libintl.h>	

namespace i18n {

	int gettext(lua_State* L)
	{
		auto str = base::i18n::gettext(luaL_checkstring(L, 1));
		lua_pushlstring(L, str.data(), str.size());
		return 1;
	}

	int textdomain(lua_State* L)
	{
		base::i18n::textdomain(luaL_checkstring(L, 1));
		return 0;
	}

	int bindtextdomain(lua_State* L)
	{
		base::i18n::bindtextdomain(luaL_checkstring(L, 1), *(fs::path*)luaL_checkudata(L, 2, "filesystem"));
		return 0;
	}
}

int luaopen_i18n(lua_State* L)
{
	luaL_Reg l[] = {
		{ "gettext", i18n::gettext },
		{ "textdomain", i18n::textdomain },
		{ "bindtextdomain", i18n::bindtextdomain },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	lua_setglobal(L, "i18n");
	return 0;
}
