#pragma once

#include <lua.hpp>
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	namespace jassbind
	{
		jass::jinteger_t read_integer     (lua_State* L, int index);
		void             push_integer     (lua_State* L, jass::jinteger_t value);
		jass::jcode_t    read_code        (lua_State* L, int index);
		void             push_code        (lua_State* L, jass::jcode_t value);
		jass::jstring_t  read_string      (lua_State* L, int index);
		void             push_string      (lua_State* L, jass::jstring_t value);
		jass::jboolean_t read_boolean     (lua_State* L, int index);
		void             push_boolean     (lua_State* L, jass::jboolean_t value);
		jass::jreal_t    read_real        (lua_State* L, int index);
		void             push_real        (lua_State* L, jass::jreal_t value);
		void             push_real_precise(lua_State* L, jass::jreal_t value);
		jass::jhandle_t  read_handle      (lua_State* L, int index);
		void             push_handle      (lua_State* L, jass::jhandle_t value);
	}
}}}
