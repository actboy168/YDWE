#pragma once

#include <lua.hpp>

namespace base { namespace lua {

	struct guard
	{
		guard(lua_State* l)
			: m_l(l)
			, m_index(lua_gettop(l))
		{ }

		~guard()
		{
			lua_settop(m_l, m_index);
		}

		lua_State* m_l;
		int m_index;
	};
}}
