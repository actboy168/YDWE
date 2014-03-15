#include "helper.h"
#include "jassbind.h"
#include "common.h"
#include <base/warcraft3/jass/func_value.h>
#include <base/lua/make_range.h>
#include <map>
#include <string>

namespace base {
	namespace lua {
		template <>
		inline int convert_to_lua(state* ls, const warcraft3::jass::func_value& v)
		{
			ls->pushunsigned((uint32_t)(uintptr_t)&v);
			ls->pushcclosure((lua::cfunction)warcraft3::lua_engine::jass_call_closure, 1);
			return 1;
		}
	}
}

namespace base { namespace warcraft3 { namespace lua_engine {

	int japi_index(lua::state* ls)
	{
		const char* name = ls->tostring(2);

		jass::func_value const* nf = jass::japi_func(name);
		if (nf)
		{
			ls->pushunsigned((uint32_t)(uintptr_t)nf);
			ls->pushcclosure((lua::cfunction)jass_call_closure, 1);
			return 1;
		}

		ls->pushnil();
		return 1;
	}

	int japi_newindex(lua::state* /*ls*/)
	{
		return 0;
	}

	int japi_pairs(lua::state* ls)
	{
		return lua::make_range(ls, jass::japi_function);
	}

	int jass_japi(lua::state* ls)
	{
		ls->newtable();
		{
			ls->newtable();
			{
				ls->pushstring("__index");
				ls->pushcclosure(japi_index, 0);
				ls->rawset(-3);

				ls->pushstring("__newindex");
				ls->pushcclosure(japi_newindex, 0);
				ls->rawset(-3);

				ls->pushstring("__pairs");
				ls->pushcclosure(japi_pairs, 0);
				ls->rawset(-3);
			}
			ls->setmetatable(-2);
		}
		return 1;
	}
}}}
