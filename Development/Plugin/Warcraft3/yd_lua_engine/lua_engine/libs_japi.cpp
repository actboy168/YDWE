#include "lua_helper.h"
#include "jassbind.h"
#include "common.h"
#include <base/warcraft3/jass/func_value.h>
#include <base/lua/make_range.h>
#include <map>
#include <string>

namespace base {
	namespace lua {
		template <>
		inline int convert_to_lua(lua_State* L, const warcraft3::jass::func_value& v)
		{
			lua_pushinteger(L, (uint32_t)(uintptr_t)&v);
			lua_pushcclosure(L, warcraft3::lua_engine::jass_call_closure, 1);
			return 1;
		}
	}
}

namespace base { namespace warcraft3 { namespace lua_engine { namespace japi {

	int japi_index(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);

		jass::func_value const* nf = jass::japi_func(name);
		if (nf)
		{
			lua_pushinteger(L, (uint32_t)(uintptr_t)nf);
			lua_pushcclosure(L, jass_call_closure, 1);
			return 1;
		}

		lua_pushnil(L);
		return 1;
	}

	int japi_newindex(lua_State* /*L*/)
	{
		return 0;
	}

	int japi_pairs(lua_State* L)
	{
		return lua::make_range(L, jass::japi_function);
	}

	int open(lua_State* L)
	{
		lua_newtable(L);
		{
			lua_newtable(L);
			{
				lua_pushstring(L, "__index");
				lua_pushcclosure(L, japi_index, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__newindex");
				lua_pushcclosure(L, japi_newindex, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__pairs");
				lua_pushcclosure(L, japi_pairs, 0);
				lua_rawset(L, -3);
			}
			lua_setmetatable(L, -2);
		}
		return 1;
	}
}}}}
