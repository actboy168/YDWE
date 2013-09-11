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
		class_<ydwe::util::conv_method>("conv_method")
			.def(constructor<uint32_t>())
			,
		def("gettext",        &ydwe::i18n::gettext),
		def("dgettext",       &ydwe::i18n::dgettext),
		def("textdomain",     &ydwe::i18n::textdomain),
		def("bindtextdomain", &ydwe::i18n::bindtextdomain),
		def("utf8_to_ansi",   &ydwe::util::u2a),
		def("ansi_to_utf8",   &ydwe::util::a2u)
	];

	return 0;
}
