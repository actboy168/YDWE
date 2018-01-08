#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)
#include <base/i18n-2/gettext.h>	
#include <base/util/unicode.h>

namespace i18n {
	static std::wstring luaL_checkwstring(lua_State* L, int idx)
	{
		size_t len = 0;
		const char* buf = luaL_checklstring(L, idx, &len);
		return base::u2w(std::string_view(buf, len), base::conv_method::replace | '?');
	}

	static int get_text(lua_State* L)
	{
		auto str = base::i18n::v2::get_text(luaL_checkstring(L, 1));
		lua_pushlstring(L, str.data(), str.size());
		return 1;
	}

	static int set_domain(lua_State* L)
	{
		base::i18n::v2::set_domain(luaL_checkwstring(L, 1));
		return 0;
	}

	static int initialize(lua_State* L)
	{
		base::i18n::v2::initialize(*(fs::path*)luaL_checkudata(L, 1, "filesystem"));
		return 0;
	}
}

int luaopen_i18n(lua_State* L)
{
	luaL_Reg l[] = {
		{ "get_text", i18n::get_text },
		{ "set_domain", i18n::set_domain },
		{ "initialize", i18n::initialize },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	lua_setglobal(L, "i18n");
	return 0;
}
