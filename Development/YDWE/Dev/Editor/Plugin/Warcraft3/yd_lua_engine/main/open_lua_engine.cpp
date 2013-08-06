#include "../main/open_lua_engine.h"
#include "../lua/callback.h"
#include "../lua/helper.h"

namespace ydwe { namespace warcraft3 { namespace lua_engine {
	int open_jass(lua::state* ls);
	int open_japi(lua::state* ls);
	int open_jass_ext(lua::state* ls);

	int open_lua_engine(lua::state* ls)
	{
		if (!lua::instance())
		{
			lua::instance() = ls;
			open_jass(ls);
			open_japi(ls);
			open_jass_ext(ls);
			lua::insert_searchers_table(ls);
		}
		return 1;
	}
}}}
