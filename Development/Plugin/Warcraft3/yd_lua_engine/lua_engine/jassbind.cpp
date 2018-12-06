#include "jassbind.h"
#include "class_real.h"
#include "callback.h"
#include <warcraft3/hashtable.h>
#include <cstdint>

namespace warcraft3::lua_engine::jassbind {
	jass::jinteger_t read_integer(lua_State* L, int index)
	{
		switch (lua_type(L, index))
		{
		case LUA_TBOOLEAN:
			return lua_toboolean(L, index) ? 1 : 0;
		case LUA_TNUMBER: {
			lua_Integer r;
			if (lua_isinteger(L, index)) {
				r = lua_tointeger(L, index);
			}
			else {
				r = (lua_Integer)lua_tonumber(L, index);
			}
			r &= 0xFFFFFFFF;
			return (jass::jinteger_t)r;
		}
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
		if (value < 0x10000) {
			value = get_jass_vm()->string_table->get(value);
		}
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
		return jass::to_real(jreal::read<float>(L, index));
	}

	void             push_real(lua_State* L, jass::jreal_t value)
	{
		lua_pushnumber(L, jass::from_real(value));
	}

	void             push_real_precise(lua_State* L, jass::jreal_t value)
	{
		jreal::push_precise(L, value);
	}
}
