#include "open_lua_engine.h"
#include "callback.h"
#include "lua_helper.h"
#include "class_real.h"
#include "class_array.h"
#include "class_handle.h"
#include <base/util/console.h>


namespace base { namespace warcraft3 { namespace lua_engine {

	int jass_common(lua::state* ls);
	int jass_japi(lua::state* ls);
	int jass_hook(lua::state* ls);
	int jass_runtime(lua::state* ls);
	int jass_slk(lua::state* ls);
	int jass_storm(lua::state* ls);
	int fix_math(lua::state* ls);

	void register_preload_lib(lua::state* ls, const char *name, lua::cfunction f)
	{
		ls->getfield(LUA_REGISTRYINDEX, "_PRELOAD");
		ls->pushcclosure(f, 0);
		ls->setfield(-2, name);
		ls->pop(1);
	}

	int open_lua_engine(lua::state* ls)
	{
		register_preload_lib(ls, "jass.common",  jass_common);
		register_preload_lib(ls, "jass.japi",    jass_japi);
		register_preload_lib(ls, "jass.hook",    jass_hook);
		register_preload_lib(ls, "jass.runtime", jass_runtime);
		register_preload_lib(ls, "jass.slk",     jass_slk);
		register_preload_lib(ls, "jass.storm",   jass_storm);

		jreal_make_mt(ls->self());
		jhandle_ud_make_mt(ls->self());
		jhandle_lud_make_mt(ls->self());
		jarray_make_mt(ls->self());

		insert_searchers_table(ls);
		fix_math(ls);

		return 0;
	}
}}}
