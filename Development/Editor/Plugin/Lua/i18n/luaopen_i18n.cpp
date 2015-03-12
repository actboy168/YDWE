#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)
#include <base/i18n/libintl.h>	
#include <base/util/unicode.h>

int luaopen_i18n(lua_State *pState)
{
	using namespace luabind;

	module(pState, "i18n")
	[
		class_<base::conv_method>("conv_method")
			.def(constructor<uint32_t>())
			,
		def("gettext",        &base::i18n::gettext),
		def("dgettext",       &base::i18n::dgettext),
		def("textdomain",     &base::i18n::textdomain),
		def("bindtextdomain", &base::i18n::bindtextdomain),
		def("utf8_to_ansi",   &base::u2a),
		def("ansi_to_utf8",   &base::a2u)
	];

	return 0;
}
