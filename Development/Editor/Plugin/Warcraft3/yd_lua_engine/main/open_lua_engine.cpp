#include "../main/open_lua_engine.h"
#include "../lua/callback.h"
#include "../lua/helper.h"
#include <base/util/console.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	int jass_common(lua_State *L);
	int jass_japi(lua_State *L);
	int jass_hook(lua_State *L);
	int jass_runtime(lua_State *L);
	int jass_slk(lua_State *L);
	int fix_math(lua::state* ls);
	int jreal_initialize(lua_State* L);

	int jass_enable_console(lua_State* /*L*/)
	{
		console::enable();
		console::disable_close_button();
		return 0;
	}

	int compatibility_layer(lua::state* ls)
	{
		ls->getglobal("require");
		ls->pushstring("jass.common");
		ls->pcall(1, 1, 0);
		ls->setglobal("jass");

		ls->getglobal("require");
		ls->pushstring("jass.japi");
		ls->pcall(1, 1, 0);
		ls->setglobal("japi");

		ls->getglobal("require");
		ls->pushstring("jass.slk");
		ls->pcall(1, 1, 0);
		ls->setglobal("slk");

		ls->newtable();
		{
			ls->pushstring("hook");
			ls->getglobal("require");
			ls->pushstring("jass.hook");
			ls->pcall(1, 1, 0);
			ls->rawset(-3);

			ls->pushstring("runtime");
			ls->getglobal("require");
			ls->pushstring("jass.runtime");
			ls->pcall(1, 1, 0);
			ls->rawset(-3);

			ls->pushstring("EnableConsole");
			ls->pushcclosure((lua::state::cfunction)jass_enable_console, 0);
			ls->rawset(-3);
		}
		ls->setglobal("jass_ext");

		return 0;
	}

	int open_lua_engine(lua::state* ls)
	{
		luaL_requiref(ls->self(), "jass.common",  jass_common, 0);
		luaL_requiref(ls->self(), "jass.japi",    jass_japi, 0);
		luaL_requiref(ls->self(), "jass.hook",    jass_hook, 0);
		luaL_requiref(ls->self(), "jass.runtime", jass_runtime, 0);
		luaL_requiref(ls->self(), "jass.slk",     jass_slk, 0);

		jreal_initialize((lua_State*)ls);
		insert_searchers_table(ls);
		fix_math(ls);

		compatibility_layer(ls);

		return 0;
	}
}}}
