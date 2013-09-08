#include "../lua/jassbind.h"
#include "../lua/class_real.h"
#include "../lua/callback.h"
#include <ydwe/warcraft3/hashtable.h>
#include <cstdint>

_BASE_BEGIN
namespace warcraft3 { namespace lua_engine {

	jass::jinteger_t jassbind::read_integer(int index) 
	{
		if (mybase::isnil(index)) return 0;

		if (mybase::isnumber(index))
			return mybase::tointeger(index);

		if (mybase::isboolean(index))
			return mybase::toboolean(index) ? 1 : 0;

		return 0;
	}

	void             jassbind::push_integer(jass::jinteger_t value)
	{
		mybase::pushinteger(value);
	}

	jass::jcode_t    jassbind::read_code   (int index) 
	{
		if (mybase::isnumber(index)) return mybase::tounsigned(index);
		if (mybase::isfunction(index)) return cfunction_to_code(this, index);
		return 0;
	}

	void             jassbind::push_code   (jass::jcode_t value)
	{
		mybase::pushunsigned(value);
	}

	jass::jstring_t    jassbind::read_string (int index) 
	{
		return jass::create_string(mybase::tostring(index));
	}

	void             jassbind::push_string (jass::jstring_t value)
	{
		mybase::pushstring(jass::from_trigstring(jass::from_string(value)));
	}

	jass::jboolean_t jassbind::read_boolean(int index) 
	{
		if (mybase::isnil(index)) return 0;

		if (mybase::isboolean(index))
			return mybase::toboolean(index) != 0;
		else
			return mybase::tonumber(index) != 0;
	}

	void             jassbind::push_boolean(jass::jboolean_t value)
	{
		mybase::pushboolean(value != 0);
	}

	jass::jreal_t    jassbind::read_real   (int index) 
	{
		if (mybase::isnil(index)) return 0;
		return jass::to_real(jreal_read<float>(self(), index));
	}
	
	void             jassbind::push_real   (jass::jreal_t value)
	{
		mybase::pushnumber(jass::from_real(value));
	}

	void             jassbind::push_real_precise(jass::jreal_t value)
	{
		jreal_push(self(), value);
	}

	jass::jhandle_t  jassbind::read_handle (int index) 
	{
		return (jass::jhandle_t)mybase::touserdata(index);
	}

	void             jassbind::push_handle (jass::jhandle_t value)
	{
		if (value)
			mybase::pushlightuserdata((void*)value);
		else
			mybase::pushnil();
	}

}}

_BASE_END
