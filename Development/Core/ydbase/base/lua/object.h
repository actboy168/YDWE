#pragma once

#include <lua.hpp>
#include <utility>

namespace base { namespace lua {

	class object
	{
	public:
		object()
			: m_l(0)
			, m_index(LUA_NOREF)
		{}

		object(lua_State* l, int idx)
			: m_l(l)
			, m_index(LUA_NOREF)
		{
			lua_pushvalue(l, idx);
			m_index = luaL_ref(l, LUA_REGISTRYINDEX);
		}

		object(object const& other)
			: m_l(other.m_l)
			, m_index(LUA_NOREF)
		{
			if (m_l == 0) return;
			lua_rawgeti(m_l, LUA_REGISTRYINDEX, other.m_index);
			m_index = luaL_ref(m_l, LUA_REGISTRYINDEX);
		}

		object(object&& other)
			: m_l(other.m_l)
			, m_index(other.m_index)
		{
			other.m_l = 0;
			other.m_index = LUA_NOREF;
		}

		~object()
		{
			if (m_l && m_index != LUA_NOREF)
				luaL_unref(m_l, LUA_REGISTRYINDEX, m_index);
		}

		object& operator=(object const& other)
		{
			object(other).swap(*this);
			return *this;
		}

		object& operator=(object&& other)
		{
			object(std::forward<object>(other)).swap(*this);
			return *this;
		}

		void swap(object& other)
		{
			std::swap(m_l, other.m_l);
			std::swap(m_index, other.m_index);
		}

		void push() const
		{
			lua_rawgeti(m_l, LUA_REGISTRYINDEX, m_index);
		}

		void push(lua_State* l) const
		{
			lua_rawgeti(l, LUA_REGISTRYINDEX, m_index);
		}

		lua_State* l() const
		{
			return m_l;
		}

		void replace(lua_State* l, int idx)
		{
			lua_pushvalue(l, idx);
			lua_rawseti(l, LUA_REGISTRYINDEX, m_index);
		}

	private:
		lua_State* m_l;
		int m_index;
	};
}}
