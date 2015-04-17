#include "open_lua_engine.h"
#include "callback.h"
#include "lua_helper.h"
#include "common.h"
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	int fix_math_random(lua::state* ls)
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

		switch (ls->gettop()) 
		{
		case 0:
			{
				ls->pushnumber(r);
				break;
			}
		case 1:
			{
				lua_Number u = ls->checknumber(1);
				ls->argcheck((lua_Number)1.0 <= u, 1, "interval is empty");
				ls->pushnumber(floor(r*u) + (lua_Number)(1.0));
				break;
			}
		case 2:
			{
				lua_Number l = ls->checknumber(1);
				lua_Number u = ls->checknumber(2);
				ls->argcheck(l <= u, 2, "interval is empty");
				ls->pushnumber(floor(r*(u-l+1)) + l);
				break;
			}
		default:
			return ls->error("wrong number of arguments");
		}
		return 1;
	}

	int fix_math_randomseed(lua::state* ls)
	{
		if (is_gaming())
		{
			jass::call("SetRandomSeed", ls->tointeger(1));
		}
		else
		{
			srand(ls->tointeger(1));
			(void)rand();
		}

		return 0;
	}

	int fix_math(lua::state* ls)
	{
		ls->getglobal("math");
		if (ls->istable(-1))
		{
			ls->pushstring("random");
			ls->pushcclosure(fix_math_random, 0);
			ls->rawset(-3);

			ls->pushstring("randomseed");
			ls->pushcclosure(fix_math_randomseed, 0);
			ls->rawset(-3);
		}
		ls->pop(1);
		return 0;
	}
}}}
