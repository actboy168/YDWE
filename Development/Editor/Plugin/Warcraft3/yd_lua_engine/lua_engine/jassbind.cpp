#include "jassbind.h"
#include "class_real.h"
#include "callback.h"
#include <base/warcraft3/hashtable.h>
#include <cstdint>

namespace base { namespace warcraft3 { namespace lua_engine {

	namespace jassbind
	{
		jass::jinteger_t read_integer(lua_State* L, int index)
		{
			switch (lua_type(L, index))
			{
			case LUA_TBOOLEAN:
				return lua_toboolean(L, index) ? 1 : 0;
			case LUA_TNUMBER:
				return (jass::jinteger_t)lua_tonumber(L, index);
			case LUA_TNIL:
			case LUA_TNONE:
			default:
				return 0;
			}
			return 0;
		}

		void             push_integer(lua_State* L, jass::jinteger_t value)
		{
			lua_pushinteger(L, value);
		}

		jass::jcode_t    read_code(lua_State* L, int index)
		{
			if (lua_isnumber(L, index)) return lua_tointeger(L, index);
			if (lua_isfunction(L, index)) return cfunction_to_code(L, index);
			return 0;
		}

		void             push_code(lua_State* L, jass::jcode_t value)
		{
			lua_pushinteger(L, value);
		}

		jass::jstring_t    read_string(lua_State* L, int index)
		{
			return jass::create_string(lua_tostring(L, index));
		}

		void             push_string(lua_State* L, jass::jstring_t value)
		{
			lua_pushstring(L, jass::from_trigstring(jass::from_string(value)));
		}

		jass::jboolean_t read_boolean(lua_State* L, int index)
		{
			if (lua_isnil(L, index)) return 0;

			if (lua_isboolean(L, index))
				return lua_toboolean(L, index) != 0;
			else
				return lua_tonumber(L, index) != 0;
		}

		void             push_boolean(lua_State* L, jass::jboolean_t value)
		{
			lua_pushboolean(L, value != 0);
		}

		jass::jreal_t    read_real(lua_State* L, int index)
		{
			if (lua_isnil(L, index)) return 0;
			return jass::to_real(jreal_read<float>(L, index));
		}

		void             push_real(lua_State* L, jass::jreal_t value)
		{
			lua_pushnumber(L, jass::from_real(value));
		}

		void             push_real_precise(lua_State* L, jass::jreal_t value)
		{
			jreal_push(L, value);
		}
	}
}}}
