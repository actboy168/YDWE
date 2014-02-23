#include <base/lua/private.h>

extern "C" {
#include <../src/lstate.h>
}

namespace base { namespace lua {
	bool allow_yield(lua_State* L)
	{
		return L->nny == 0;
	}
}}
