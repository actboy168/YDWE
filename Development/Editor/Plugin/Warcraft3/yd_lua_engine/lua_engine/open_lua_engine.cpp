#include "open_lua_engine.h"
#include "callback.h"
#include "lua_helper.h"
#include "class_real.h"
#include "class_array.h"
#include "class_handle.h"
#include <base/util/console.h> 
#include <base/util/unicode.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	namespace common { int open(lua_State* L); }
	namespace globals { int open(lua_State* L); }
	namespace japi { int open(lua_State* L); }
	namespace hook { int open(lua_State* L); }
	namespace runtime { int open(lua_State* L); }
	namespace slk { int open(lua_State* L); }
	namespace storm { int open(lua_State* L); }
	namespace console { int open(lua_State* L); }
	namespace debug { int open(lua_State* L); }
	namespace message { int open(lua_State* L); }
	namespace bignum { int open(lua_State* L); }
	namespace log { int open(lua_State* L); }

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
		register_preload_lib(L, "jass.common",  common::open);
		register_preload_lib(L, "jass.globals", globals::open);
		register_preload_lib(L, "jass.japi",    japi::open);
		register_preload_lib(L, "jass.hook",    hook::open);
		register_preload_lib(L, "jass.runtime", runtime::open);
		register_preload_lib(L, "jass.slk",     slk::open);
		register_preload_lib(L, "jass.storm",   storm::open);
		register_preload_lib(L, "jass.console", console::open);
		register_preload_lib(L, "jass.debug",   debug::open);
		register_preload_lib(L, "jass.message", message::open);
		register_preload_lib(L, "jass.bignum",  bignum::open);	
		register_preload_lib(L, "jass.log",     log::open);

		jreal_make_mt(L);
		jhandle_ud_make_mt(L);
		jhandle_lud_make_mt(L);
		jarray_make_mt(L);

		insert_searchers_table(L);
		fix_baselib(L);

		return 0;
	}
}}}
