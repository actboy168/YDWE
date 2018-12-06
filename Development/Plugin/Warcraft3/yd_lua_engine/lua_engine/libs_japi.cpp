#include "jassbind.h"
#include "common.h"
#include <warcraft3/jass/func_value.h>
#include <bee/lua/range.h>
#include <map>
#include <string>
#include <warcraft3/jass/hook.h>
#include "lua_to_nativefunction.h"

namespace bee::lua {
	template <>
	inline int convert_to_lua(lua_State* L, const base::warcraft3::jass::func_value& v)
	{
		lua_pushinteger(L, (uint32_t)(uintptr_t)&v);
		lua_pushcclosure(L, base::warcraft3::lua_engine::jass_call_closure, 1);
		return 1;
	}
}


namespace base { namespace warcraft3 { namespace lua_engine { namespace japi {

	int empty_function(lua_State*, void*)
	{
		return 0;
	}

	struct japi
	{
		typedef lua_to_nativefunction<empty_function> mybase;

		jass::func_value funcdef;
		japi(lua_State*L, int luaobj, const char* name, const char* def)
			: funcdef(def, 0)
			, base_(new mybase(L, luaobj, &funcdef, 0))
		{
			jass::japi_table_add((uintptr_t)base_->funcentry, name, def);
		}

		~japi() { delete base_; }
		static const char* name() { return "japi_t"; }

		mybase* base_;
	};

	int japi_index(lua_State* L)
	{
		const char* name = luaL_checkstring(L, 2);
		jass::func_value const* nf = jass::japi_func(name);
		if (nf) {
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

	int japi_call(lua_State* L)
	{
		const char* name = luaL_checkstring(L, 2);
		const char* def = luaL_checkstring(L, 3);
		luaL_checktype(L, 4, LUA_TFUNCTION);
		if (jass::japi_func(name)) {
			return 0;
		}

		runtime::get_global_table(L, "_JASS_JAPI_TABLE", false);
		lua_pushstring(L, name);
		lua_rawget(L, -2);
		japi* self = (japi*)luaL_testudata(L, -1, japi::name());
		if (self) {
			return 0;
		}
		lua_pop(L, 1);
		lua_pushstring(L, name);
		self = (japi*)lua_newuserdata(L, sizeof(japi));
		int luaobj = runtime::callback_push(L, 4);
		new (self)japi(L, luaobj, name, def);
		luaL_setmetatable(L, japi::name());
		lua_rawset(L, -3);
		return 0;
	}

	int japi_pairs(lua_State* L)
	{
		return bee::lua::make_range(L, jass::japi_function);
	}

	static int mt_gc(lua_State *L)
	{
		japi* self = (japi*)luaL_testudata(L, 1, japi::name());
		if (self) {
			self->~japi();
		}
		return 0;
	}

	int open(lua_State* L)
	{
		luaL_Reg mt1[] = {
			{ "__gc", mt_gc },
			{ NULL, NULL },
		};

		luaL_newmetatable(L, japi::name());
		luaL_setfuncs(L, mt1, 0);
		lua_pop(L, 1);

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

				lua_pushstring(L, "__call");
				lua_pushcclosure(L, japi_call, 0);
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
