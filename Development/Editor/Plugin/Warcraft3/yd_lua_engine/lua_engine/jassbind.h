#pragma once

#include <base/lua/state.h>
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	namespace jassbind
	{
		jass::jinteger_t read_integer     (lua::state* ls, int index);
		void             push_integer     (lua::state* ls, jass::jinteger_t value);
		jass::jcode_t    read_code        (lua::state* ls, int index);
		void             push_code        (lua::state* ls, jass::jcode_t value);
		jass::jstring_t  read_string      (lua::state* ls, int index);
		void             push_string      (lua::state* ls, jass::jstring_t value);
		jass::jboolean_t read_boolean     (lua::state* ls, int index);
		void             push_boolean     (lua::state* ls, jass::jboolean_t value);
		jass::jreal_t    read_real        (lua::state* ls, int index);
		void             push_real        (lua::state* ls, jass::jreal_t value);
		void             push_real_precise(lua::state* ls, jass::jreal_t value);
		jass::jhandle_t  read_handle      (lua::state* ls, int index);
		void             push_handle      (lua::state* ls, jass::jhandle_t value);
	}
}}}
