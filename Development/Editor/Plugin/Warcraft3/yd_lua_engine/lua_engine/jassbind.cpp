#include "jassbind.h"
#include "class_real.h"
#include "callback.h"
#include <base/warcraft3/hashtable.h>
#include <cstdint>

namespace base { namespace warcraft3 { namespace lua_engine {

	namespace jassbind
	{
		jass::jinteger_t read_integer(lua::state* ls, int index)
		{
			if (ls->isnil(index)) return 0;

			if (ls->isnumber(index))
				return ls->tointeger(index);

			if (ls->isboolean(index))
				return ls->toboolean(index) ? 1 : 0;

			return 0;
		}

		void             push_integer(lua::state* ls, jass::jinteger_t value)
		{
			ls->pushinteger(value);
		}

		jass::jcode_t    read_code(lua::state* ls, int index)
		{
			if (ls->isnumber(index)) return ls->tounsigned(index);
			if (ls->isfunction(index)) return cfunction_to_code(ls, index);
			return 0;
		}

		void             push_code(lua::state* ls, jass::jcode_t value)
		{
			ls->pushunsigned(value);
		}

		jass::jstring_t    read_string(lua::state* ls, int index)
		{
			return jass::create_string(ls->tostring(index));
		}

		void             push_string(lua::state* ls, jass::jstring_t value)
		{
			ls->pushstring(jass::from_trigstring(jass::from_string(value)));
		}

		jass::jboolean_t read_boolean(lua::state* ls, int index)
		{
			if (ls->isnil(index)) return 0;

			if (ls->isboolean(index))
				return ls->toboolean(index) != 0;
			else
				return ls->tonumber(index) != 0;
		}

		void             push_boolean(lua::state* ls, jass::jboolean_t value)
		{
			ls->pushboolean(value != 0);
		}

		jass::jreal_t    read_real(lua::state* ls, int index)
		{
			if (ls->isnil(index)) return 0;
			return jass::to_real(jreal_read<float>(ls->self(), index));
		}

		void             push_real(lua::state* ls, jass::jreal_t value)
		{
			ls->pushnumber(jass::from_real(value));
		}

		void             push_real_precise(lua::state* ls, jass::jreal_t value)
		{
			jreal_push(ls->self(), value);
		}
	}
}}}
