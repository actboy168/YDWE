#pragma once

#include <ydwe/lua/state.h>
#include <ydwe/warcraft3/jass.h>

_BASE_BEGIN
namespace warcraft3 { namespace lua_engine {

	struct jassbind : public lua::state
	{
		typedef lua::state mybase;

		jass::jinteger_t read_integer(int index);
		void             push_integer(jass::jinteger_t value);
		jass::jcode_t    read_code   (int index);
		void             push_code   (jass::jcode_t value);
		jass::jstring_t  read_string (int index);
		void             push_string (jass::jstring_t value);
		jass::jboolean_t read_boolean(int index);
		void             push_boolean(jass::jboolean_t value);
		jass::jreal_t    read_real   (int index);
		void             push_real   (jass::jreal_t value);
		void             push_real_precise(jass::jreal_t value);
		jass::jhandle_t  read_handle (int index);
		void             push_handle (jass::jhandle_t value);
	};
}}

_BASE_END
