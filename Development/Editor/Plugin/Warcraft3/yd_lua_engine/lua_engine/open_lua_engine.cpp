#include "open_lua_engine.h"
#include "callback.h"
#include "lua_helper.h"
#include "class_real.h"
#include "class_array.h"
#include "class_handle.h"
#include <base/util/console.h> 
#include <base/util/unicode.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	int jass_common(lua::state* ls); 
	int jass_globals(lua::state* ls);
	int jass_japi(lua::state* ls);
	int jass_hook(lua::state* ls);
	int jass_runtime(lua::state* ls);
	int jass_slk(lua::state* ls);
	int jass_storm(lua::state* ls);
	int jass_console(lua::state* ls);
	int fix_math(lua::state* ls);

	void register_preload_lib(lua::state* ls, const char *name, lua::cfunction f)
	{
		ls->getfield(LUA_REGISTRYINDEX, "_PRELOAD");
		ls->pushcclosure(f, 0);
		ls->setfield(-2, name);
		ls->pop(1);
	}

	int fix_os(lua::state* ls)
	{
		ls->getglobal("os");
		if (ls->istable(-1))
		{
			ls->pushstring("execute");   ls->pushnil(); ls->rawset(-3);
			ls->pushstring("exit");      ls->pushnil(); ls->rawset(-3);
			ls->pushstring("getenv");    ls->pushnil(); ls->rawset(-3);
			ls->pushstring("remove");    ls->pushnil(); ls->rawset(-3);
			ls->pushstring("rename");    ls->pushnil(); ls->rawset(-3);
			ls->pushstring("setlocale"); ls->pushnil(); ls->rawset(-3);
			ls->pushstring("tmpname");   ls->pushnil(); ls->rawset(-3);
		}
		ls->pop(1);
		return 0;
	}

	int open_lua_engine(lua::state* ls)
	{
		register_preload_lib(ls, "jass.common",  jass_common);
		register_preload_lib(ls, "jass.globals", jass_globals);
		register_preload_lib(ls, "jass.japi",    jass_japi);
		register_preload_lib(ls, "jass.hook",    jass_hook);
		register_preload_lib(ls, "jass.runtime", jass_runtime);
		register_preload_lib(ls, "jass.slk",     jass_slk);
		register_preload_lib(ls, "jass.storm",   jass_storm);
		register_preload_lib(ls, "jass.console", jass_console);

		jreal_make_mt(ls->self());
		jhandle_ud_make_mt(ls->self());
		jhandle_lud_make_mt(ls->self());
		jarray_make_mt(ls->self());

		insert_searchers_table(ls);
		fix_math(ls);
		fix_os(ls);

		return 0;
	}
}}}
