#include "allow_yield.h"

extern "C" {
#include <lstate.h>
}

namespace base { namespace lua {
	bool allow_yield(lua_State* L)
	{
		return L->nny == 0;
	}
}}
