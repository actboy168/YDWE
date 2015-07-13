#include "open_lua_engine.h"
#include "callback.h"
#include "lua_helper.h"
#include "class_real.h"
#include "class_array.h"
#include "class_handle.h"
#include <base/util/console.h> 
#include <base/util/unicode.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	int jass_common(lua_State* L); 
	int jass_globals(lua_State* L);
	int jass_japi(lua_State* L);
	int jass_hook(lua_State* L);
	int jass_runtime(lua_State* L);
	int jass_slk(lua_State* L);
	int jass_storm(lua_State* L);
	int jass_console(lua_State* L);
	int jass_debug(lua_State* L);
	int fix_math(lua_State* L);

	void register_preload_lib(lua_State* L, const char *name, lua_CFunction f)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "_PRELOAD");
		lua_pushcclosure(L, f, 0);
		lua_setfield(L, -2, name);
		lua_pop(L, 1);
	}

	int fix_baselib(lua_State* L)
	{
		fix_math(L);

		lua_getglobal(L, "os");
		if (lua_istable(L, -1))
		{
			lua_pushstring(L, "execute");   lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "exit");      lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "getenv");    lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "remove");    lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "rename");    lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "setlocale"); lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "tmpname");   lua_pushnil(L); lua_rawset(L, -3);
		}
		lua_pop(L, 1);

		lua_getglobal(L, "package");
		if (lua_istable(L, -1))
		{
			lua_pushstring(L, "loadlib");   lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "searchpath");lua_pushnil(L); lua_rawset(L, -3);
		}
		lua_pop(L, 1);

		lua_pushnil(L);
		lua_setglobal(L, "dofile");
		lua_pushnil(L);
		lua_setglobal(L, "loadfile");
		return 0;
	}

	int open_lua_engine(lua_State* L)
	{
		register_preload_lib(L, "jass.common",  jass_common);
		register_preload_lib(L, "jass.globals", jass_globals);
		register_preload_lib(L, "jass.japi",    jass_japi);
		register_preload_lib(L, "jass.hook",    jass_hook);
		register_preload_lib(L, "jass.runtime", jass_runtime);
		register_preload_lib(L, "jass.slk",     jass_slk);
		register_preload_lib(L, "jass.storm",   jass_storm);
		register_preload_lib(L, "jass.console", jass_console);
		register_preload_lib(L, "jass.debug",   jass_debug);

		jreal_make_mt(L);
		jhandle_ud_make_mt(L);
		jhandle_lud_make_mt(L);
		jarray_make_mt(L);

		insert_searchers_table(L);
		fix_baselib(L);

		return 0;
	}
}}}
