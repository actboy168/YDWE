#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/filesystem.h>

namespace luabind
{
	template <>
	struct default_converter<fs::path>
		: native_converter_base<fs::path>
	{
		static int compute_score(lua_State* L, int index)
		{
			return luaL_testudata(L, index, "filesystem")? 0 : -1;
		}

		fs::path from(lua_State* L, int index)
		{
			return *(fs::path*)luaL_checkudata(L, index, "filesystem");
		}

		void to(lua_State* L, fs::path const& value)
		{
			void* storage = lua_newuserdata(L, sizeof(fs::path));
			luaL_getmetatable(L, "filesystem");
			lua_setmetatable(L, -2);
			new (storage)fs::path(value);
		}
	};

	template <>
	struct default_converter<fs::path const>
		: default_converter<fs::path>
	{};

	template <>
	struct default_converter<fs::path const&>
		: default_converter<fs::path>
	{};
} 
