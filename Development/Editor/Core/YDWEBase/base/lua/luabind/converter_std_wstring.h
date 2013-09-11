#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/util/unicode.h>

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
			return ydwe::util::a2w(std::string(lua_tostring(L, index), lua_strlen(L, index)));
		}

		void to(lua_State* L, std::wstring const& value)
		{
			std::string utf8_value = ydwe::util::w2a(value);
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
