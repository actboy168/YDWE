#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <ydwe/util/string_ref.h>

namespace luabind
{
	template <>
	struct default_converter<boost::string_ref>
		: native_converter_base<boost::string_ref>
	{
		static int compute_score(lua_State* L, int index)
		{
			return lua_type(L, index) == LUA_TSTRING ? 0 : -1;
		}

		boost::string_ref from(lua_State* L, int index)
		{
			return boost::string_ref(lua_tostring(L, index), lua_strlen(L, index));
		}

		void to(lua_State* L, boost::string_ref const& value)
		{
			lua_pushlstring(L, value.data(), value.size());
		}
	};

	template <>
	struct default_converter<boost::string_ref const>
		: default_converter<boost::string_ref>
	{};

	template <>
	struct default_converter<boost::string_ref const&>
		: default_converter<boost::string_ref>
	{};
} 
