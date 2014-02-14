#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>

namespace luabind
{
	template <>
	struct default_converter<void *>
	{
		typedef boost::mpl::true_ is_native;

		int const consumed_args(...)
		{
			return 1;
		}

		template <class U>
		static int match(lua_State *L, U, int index)
		{
			int type = lua_type(L, index);
			return (type == LUA_TLIGHTUSERDATA || type == LUA_TUSERDATA || type == LUA_TNIL) ? 0 : -1;
		}

		template <class U>
		void *apply(lua_State *L, U, int index)
		{
			return lua_touserdata(L, index);
		}

		void apply(lua_State *L, void *value)
		{
			if (value)
				lua_pushlightuserdata(L, value);
			else
				lua_pushnil(L);
		}

		template <class U>
		void converter_postcall(lua_State *, U, int)
		{}
	};

	template <>
	struct default_converter<const void *>
		: default_converter<void *>
	{};

	template <>
	struct default_converter<void * const>
		: default_converter<void *>
	{};

} 
