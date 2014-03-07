#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/util/string_view.h>

namespace luabind
{
	template <>
	struct default_converter<std::string_view>
		: native_converter_base<std::string_view>
	{
		static int compute_score(lua_State* L, int index)
		{
			return lua_type(L, index) == LUA_TSTRING ? 0 : -1;
		}

		std::string_view from(lua_State* L, int index)
		{
			return std::string_view(lua_tostring(L, index), lua_rawlen(L, index));
		}

		void to(lua_State* L, std::string_view const& value)
		{
			lua_pushlstring(L, value.data(), value.size());
		}
	};

	template <>
	struct default_converter<std::string_view const>
		: default_converter<std::string_view>
	{};

	template <>
	struct default_converter<std::string_view const&>
		: default_converter<std::string_view>
	{};
} 
