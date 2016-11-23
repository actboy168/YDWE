#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)
#include <base/i18n/libintl.h>	

int luaopen_i18n(lua_State *pState)
{
	using namespace luabind;

	module(pState, "i18n")
	[
		def("gettext",        &base::i18n::gettext),
		def("dgettext",       &base::i18n::dgettext),
		def("textdomain",     &base::i18n::textdomain),
		def("bindtextdomain", &base::i18n::bindtextdomain)
	];

	return 0;
}
