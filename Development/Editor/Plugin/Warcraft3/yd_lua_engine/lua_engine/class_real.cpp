#include "class_real.h"
#include "lua_helper.h"
#include "jassbind.h"
#include <base/util/format.h>

namespace base { namespace warcraft3 { namespace lua_engine {

#define LUA_JASS_REAL "jreal_t"

	jass::jreal_t* jreal_create(lua_State* L)
	{
		jass::jreal_t* p = (jass::jreal_t*)lua_newuserdata(L, sizeof(jass::jreal_t));
		*p = 0;
		luaL_getmetatable(L, LUA_JASS_REAL);
		lua_setmetatable(L, -2);
		return p;
	}

	bool jreal_precise(lua_State* L, int index1)
	{
		return (lua_type(L, index1) == LUA_TUSERDATA);
	}

	bool jreal_precise(lua_State* L, int index1, int index2)
	{
		return ((lua_type(L, index1) == LUA_TUSERDATA) && (lua_type(L, index2) == LUA_TUSERDATA));
	}

	template <class T>
	T jreal_read(lua_State* L, int index) 
	{
		int type = lua_type(L, index);
		T f = 0;

		switch(type)
		{
		case LUA_TNUMBER: 
			{
				f = (T)lua_tonumber(L, index);
				break;						 
			}
		case LUA_TUSERDATA:
			{
				jass::jreal_t tmp = *(jass::jreal_t*)luaL_checkudata(L, index, LUA_JASS_REAL);
				f = (T)jass::from_real(tmp);
				break;								 
			}
		default:
			luaL_error(L, "argument %d error type %s", index, lua_typename(L, type));
			break;	
		}
		return f;
	}

	void jreal_push(lua_State* L, float f)
	{
		jass::jreal_t* p = jreal_create(L);
		*p = jass::to_real(f);
	}

	void jreal_push(lua_State* L, jass::jreal_t f)
	{
		jass::jreal_t* p = jreal_create(L);
		*p = f;
	}

	int jreal_add(lua_State* L)
	{
		if (jreal_precise(L, 1, 2))
		{
			jreal_push(L, jreal_read<float>(L, 1) + jreal_read<float>(L, 2));
		}
		else
		{
			lua_pushnumber(L, jreal_read<lua_Number>(L, 1) + jreal_read<lua_Number>(L, 2));
		}

		return 1;
	}

	int jreal_sub(lua_State* L)
	{
		if (jreal_precise(L, 1, 2))
		{
			jreal_push(L, jreal_read<float>(L, 1) - jreal_read<float>(L, 2));
		}
		else
		{
			lua_pushnumber(L, jreal_read<lua_Number>(L, 1) - jreal_read<lua_Number>(L, 2));
		}

		return 1;
	}

	int jreal_mul(lua_State* L)
	{
		if (jreal_precise(L, 1, 2))
		{
			jreal_push(L, jreal_read<float>(L, 1) * jreal_read<float>(L, 2));
		}
		else
		{
			lua_pushnumber(L, jreal_read<lua_Number>(L, 1) * jreal_read<lua_Number>(L, 2));
		}

		return 1;
	}

	int jreal_div(lua_State* L)
	{
		if (jreal_precise(L, 1, 2))
		{
			jreal_push(L, jreal_read<float>(L, 1) / jreal_read<float>(L, 2));
		}
		else
		{
			lua_pushnumber(L, jreal_read<lua_Number>(L, 1) / jreal_read<lua_Number>(L, 2));
		}
		return 1;
	}
	template <class T>
	T jreal_math_mod(lua_State* L, int index_a, int index_b)
	{
		T a = jreal_read<T>(L, index_a);
		T b = jreal_read<T>(L, index_b);
		return a - floor(a/b)*b;
	}

	int jreal_mod(lua_State* L)
	{
		if (jreal_precise(L, 1, 2))
		{
			jreal_push(L, jreal_math_mod<float>(L, 1, 2));
		}
		else
		{
			lua_pushnumber(L, jreal_math_mod<lua_Number>(L, 1, 2));
		}
		return 1;
	}

	int jreal_unm(lua_State* L)
	{
		if (jreal_precise(L, 1))
		{
			jreal_push(L, -jreal_read<float>(L, 1));
		}
		else
		{
			lua_pushnumber(L, -jreal_read<lua_Number>(L, 1));
		}
		return 1;
	}

	int jreal_pow(lua_State* L)
	{
		if (jreal_precise(L, 1, 2))
		{
			jreal_push(L, pow(jreal_read<float>(L, 1), jreal_read<float>(L, 2)));
		}
		else
		{
			lua_pushnumber(L, pow(jreal_read<lua_Number>(L, 1), jreal_read<lua_Number>(L, 2)));
		}
		return 1;
	}

	int jreal_eq(lua_State* L)
	{
		if (jreal_precise(L, 1, 2))
		{
			lua_pushboolean(L, jreal_read<float>(L, 1) == jreal_read<float>(L, 2));
		}
		else
		{
			lua_pushboolean(L, jreal_read<lua_Number>(L, 1) == jreal_read<lua_Number>(L, 2));
		}
		return 1;
	}

	int jreal_lt(lua_State* L)
	{
		if (jreal_precise(L, 1, 2))
		{
			lua_pushboolean(L, jreal_read<float>(L, 1) < jreal_read<float>(L, 2));
		}
		else
		{
			lua_pushboolean(L, jreal_read<lua_Number>(L, 1) < jreal_read<lua_Number>(L, 2));
		}
		return 1;
	}

	int jreal_le(lua_State* L)
	{
		if (jreal_precise(L, 1, 2))
		{
			lua_pushboolean(L, jreal_read<float>(L, 1) <= jreal_read<float>(L, 2));
		}
		else
		{
			lua_pushboolean(L, jreal_read<lua_Number>(L, 1) <= jreal_read<lua_Number>(L, 2));
		}
		return 1;
	}

	int jreal_len(lua_State *L) 
	{
		if (jreal_precise(L, 1))
		{
			lua_pushnumber(L, jreal_read<float>(L, 1));
		}
		else
		{
			lua_pushvalue(L, 1);
		}
		return 1;
	}

	int jreal_tostring(lua_State* L)
	{		
		lua_pushstring(L, format("%f", jreal_read<float>(L, 1)).c_str());
		return 1;
	}

	void jreal_make_mt(lua_State* L)
	{	
		luaL_Reg lib[] = {
			{ "__add",      jreal_add      },
			{ "__sub",      jreal_sub      },
			{ "__mul",      jreal_mul      },
			{ "__div",      jreal_div      },
			{ "__mod",      jreal_mod      },
			{ "__unm",      jreal_unm      },
			{ "__pow",      jreal_pow      },
			{ "__eq",       jreal_eq       },
			{ "__lt",       jreal_lt       },
			{ "__le",       jreal_le       },
			{ "__len",      jreal_len      },
			{ "__tostring", jreal_tostring },
			{ NULL, NULL },
		};

		luaL_newmetatable(L, LUA_JASS_REAL);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
#if LUA_VERSION_NUM >= 502
		luaL_setfuncs(L, lib, 0);
		lua_pop(L, 1);
#else
		luaL_register(L, NULL, lib);
#endif
	}
}}}
