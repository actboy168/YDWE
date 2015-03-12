#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/util/unicode.h>
#include <base/util/string_view.h>

namespace luabind
{
	template <>
	struct default_converter<std::wstring>
		: native_converter_base<std::wstring>
	{
		static int compute_score(lua_State* L, int index)
		{
			return lua_type(L, index) == LUA_TSTRING ? 0 : -1;
		}

		std::wstring from(lua_State* L, int index)
		{
			return base::u2w(std::string_view(lua_tostring(L, index), lua_rawlen(L, index)), base::conv_method::replace | '?');
		}

		void to(lua_State* L, std::wstring const& value)
		{
			std::string utf8_value = base::w2u(value, base::conv_method::replace | '?');
			lua_pushlstring(L, utf8_value.data(), utf8_value.size());
		}
	};

	template <>
	struct default_converter<std::wstring const>
		: default_converter<std::wstring>
	{};

	template <>
	struct default_converter<std::wstring const&>
		: default_converter<std::wstring>
	{};
} 
