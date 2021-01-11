#include "class_real.h"
#include "jassbind.h"
#include <fmt/format.h>

namespace warcraft3::lua_engine::jreal {

#define LUA_JASS_REAL "jreal_t"

	static bool is_precise(lua_State* L, int index1, int index2)
	{
		return ((lua_type(L, index1) == LUA_TUSERDATA) && (lua_type(L, index2) == LUA_TUSERDATA));
	}

	template <class T>
	T read(lua_State* L, int index)
	{
		int type = lua_type(L, index);
		switch (type)
		{
		case LUA_TNUMBER:
			return (T)lua_tonumber(L, index);
		case LUA_TUSERDATA:
			return (T)jass::from_real(*(jass::jreal_t*)luaL_checkudata(L, index, LUA_JASS_REAL));
		default:
			luaL_error(L, "argument %d error type %s", index, lua_typename(L, type));
			break;
		}
		return 0;
	}

	static jass::jreal_t* create(lua_State* L)
	{
		jass::jreal_t* p = (jass::jreal_t*)lua_newuserdata(L, sizeof(jass::jreal_t));
		*p = 0;
		luaL_getmetatable(L, LUA_JASS_REAL);
		lua_setmetatable(L, -2);
		return p;
	}

	void push_precise(lua_State* L, jass::jreal_t f)
	{
		jass::jreal_t* p = create(L);
		*p = f;
	}

	template <class T>
	void push(lua_State* L, T f);

	template <>
	void push<float>(lua_State* L, float f)
	{
		jass::jreal_t* p = create(L);
		*p = jass::to_real(f);
	}

	template <>
	void push<lua_Number>(lua_State* L, lua_Number f)
	{
		lua_pushnumber(L, f);
	}

	static int mt_add(lua_State* L)
	{
		if (is_precise(L, 1, 2))
		{
			push(L, read<float>(L, 1) + read<float>(L, 2));
		}
		else
		{
			push(L, read<lua_Number>(L, 1) + read<lua_Number>(L, 2));
		}
		return 1;
	}

	static int mt_sub(lua_State* L)
	{
		if (is_precise(L, 1, 2))
		{
			push(L, read<float>(L, 1) - read<float>(L, 2));
		}
		else
		{
			push(L, read<lua_Number>(L, 1) - read<lua_Number>(L, 2));
		}
		return 1;
	}

	static int mt_mul(lua_State* L)
	{
		if (is_precise(L, 1, 2))
		{
			push(L, read<float>(L, 1) * read<float>(L, 2));
		}
		else
		{
			push(L, read<lua_Number>(L, 1) * read<lua_Number>(L, 2));
		}
		return 1;
	}

	static int mt_div(lua_State* L)
	{
		if (is_precise(L, 1, 2))
		{
			push(L, read<float>(L, 1) / read<float>(L, 2));
		}
		else
		{
			push(L, read<lua_Number>(L, 1) / read<lua_Number>(L, 2));
		}
		return 1;
	}

	template <class T>
	static int template_mod(lua_State* L)
	{
		T a = read<T>(L, 1);
		T b = read<T>(L, 2);
		push<T>(L, a - floor(a / b) * b);
		return 1;
	}

	static int mt_mod(lua_State* L)
	{
		if (is_precise(L, 1, 2))
			return template_mod<float>(L);
		else
			return template_mod<lua_Number>(L);
	}

	static int mt_pow(lua_State* L)
	{
		if (is_precise(L, 1, 2))
		{
			push(L, pow(read<float>(L, 1), read<float>(L, 2)));
		}
		else
		{
			push(L, pow(read<lua_Number>(L, 1), read<lua_Number>(L, 2)));
		}
		return 1;
	}

	static int mt_eq(lua_State* L)
	{
		if (is_precise(L, 1, 2))
		{
			lua_pushboolean(L, read<float>(L, 1) == read<float>(L, 2));
		}
		else
		{
			lua_pushboolean(L, read<lua_Number>(L, 1) == read<lua_Number>(L, 2));
		}
		return 1;
	}

	static int mt_lt(lua_State* L)
	{
		if (is_precise(L, 1, 2))
		{
			lua_pushboolean(L, read<float>(L, 1) < read<float>(L, 2));
		}
		else
		{
			lua_pushboolean(L, read<lua_Number>(L, 1) < read<lua_Number>(L, 2));
		}
		return 1;
	}

	static int mt_le(lua_State* L)
	{
		if (is_precise(L, 1, 2))
		{
			lua_pushboolean(L, read<float>(L, 1) <= read<float>(L, 2));
		}
		else
		{
			lua_pushboolean(L, read<lua_Number>(L, 1) <= read<lua_Number>(L, 2));
		}
		return 1;
	}

	static int mt_unm(lua_State* L)
	{
		push(L, -read<float>(L, 1));
		return 1;
	}

	static int mt_tostring(lua_State* L)
	{
		lua_pushfstring(L, "%f", read<float>(L, 1));
		return 1;
	}

	void init(lua_State* L)
	{
		luaL_Reg lib[] = {
			{ "__add",      mt_add },
			{ "__sub",      mt_sub },
			{ "__mul",      mt_mul },
			{ "__div",      mt_div },
			{ "__mod",      mt_mod },
			{ "__unm",      mt_unm },
			{ "__pow",      mt_pow },
			{ "__eq",       mt_eq },
			{ "__lt",       mt_lt },
			{ "__le",       mt_le },
			{ "__tostring", mt_tostring },
			{ NULL, NULL },
		};

		luaL_newmetatable(L, LUA_JASS_REAL);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, lib, 0);
		lua_pop(L, 1);
	}
}
