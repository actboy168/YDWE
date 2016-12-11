#include "callback.h"
#include "lua_helper.h"
#include "common.h"
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	int fix_math_random(lua_State* L)
	{
		lua_Integer low = 0, up = 1;
		double r;
		if (is_gaming())
		{
			jass::jreal_t a = jass::to_real(0.f);
			jass::jreal_t b = jass::to_real(1.f);
			r = (double)jass::from_real(jass::call("GetRandomReal", &a, &b));
		}
		else
		{
			r = (double)rand() * (1.0 / ((double)RAND_MAX + 1.0));
		}

		switch (lua_gettop(L)) 
		{
		case 0:
			lua_pushnumber(L, (lua_Number)r);
			break;
		case 1:
			low = 1;
			up = luaL_checkinteger(L, 1);
			break;
		case 2:
			low = luaL_checkinteger(L, 1);
			up = luaL_checkinteger(L, 2);
			break;
		default:
			return luaL_error(L, "wrong number of arguments");
		}
		luaL_argcheck(L, low <= up, 1, "interval is empty");
		luaL_argcheck(L, low >= 0 || up <= LUA_MAXINTEGER + low, 1, "interval too large");
		r *= (double)(up - low) + 1.0;
		lua_pushinteger(L, (lua_Integer)r + low);
		return 1;
	}

	int fix_math_randomseed(lua_State* L)
	{
		if (is_gaming())
		{
			jass::call("SetRandomSeed", (unsigned int)(lua_Integer)luaL_checknumber(L, 1));
		}
		else
		{
			srand((unsigned int)(lua_Integer)luaL_checknumber(L, 1));
			(void)rand();
		}

		return 0;
	}

	int fix_math(lua_State* L)
	{
		lua_getglobal(L, "math");
		if (lua_istable(L, -1))
		{
			lua_pushstring(L, "random");
			lua_pushcclosure(L, fix_math_random, 0);
			lua_rawset(L, -3);

			lua_pushstring(L, "randomseed");
			lua_pushcclosure(L, fix_math_randomseed, 0);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
		return 0;
	}
}}}
