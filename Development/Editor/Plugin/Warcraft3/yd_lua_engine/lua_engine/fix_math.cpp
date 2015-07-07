#include "callback.h"
#include "lua_helper.h"
#include "common.h"
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	int fix_math_random(lua_State* L)
	{
		lua_Number r;

		if (is_gaming())
		{
			jass::jreal_t a = jass::to_real(0.f);
			jass::jreal_t b = jass::to_real(1.f);
			r = (lua_Number)jass::from_real(jass::call("GetRandomReal", &a, &b));
		}
		else
		{
			r = (lua_Number)(rand()%RAND_MAX) / (lua_Number)RAND_MAX;
		}

		switch (lua_gettop(L)) 
		{
		case 0:
			{
				lua_pushnumber(L, r);
				break;
			}
		case 1:
			{
				lua_Number u = luaL_checknumber(L, 1);
				luaL_argcheck(L, (lua_Number)1.0 <= u, 1, "interval is empty");
				lua_pushnumber(L, floor(r*u) + (lua_Number)(1.0));
				break;
			}
		case 2:
			{
				lua_Number l = luaL_checknumber(L, 1);
				lua_Number u = luaL_checknumber(L, 2);
				luaL_argcheck(L, l <= u, 2, "interval is empty");
				lua_pushnumber(L, floor(r*(u - l + 1)) + l);
				break;
			}
		default:
			return luaL_error(L, "wrong number of arguments");
		}
		return 1;
	}

	int fix_math_randomseed(lua_State* L)
	{
		if (is_gaming())
		{
			jass::call("SetRandomSeed", lua_tointeger(L, 1));
		}
		else
		{
			srand(lua_tointeger(L, 1));
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
