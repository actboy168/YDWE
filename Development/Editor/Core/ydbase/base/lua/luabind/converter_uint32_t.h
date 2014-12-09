#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <cstdint>

namespace luabind
{
	template <>
	struct default_converter<uint32_t>
		: native_converter_base<uint32_t>
	{
		static int compute_score(lua_State* L, int index)
		{
			return lua_type(L, index) == LUA_TNUMBER ? 0 : -1;
		}

		uint32_t from(lua_State* L, int index)
		{
			return 0xFFFFFFFF & (lua_Unsigned)luaL_checkinteger(L, index);
		}

		void to(lua_State* L, uint32_t const& value)
		{
			lua_pushinteger(L, (lua_Integer)value);
		}
	};

	template <>
	struct default_converter<uint32_t const>
		: default_converter<uint32_t>
	{};

	template <>
	struct default_converter<uint32_t const&>
		: default_converter<uint32_t>
	{};
}
