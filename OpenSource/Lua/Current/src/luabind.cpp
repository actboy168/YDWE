#include <luabind/open.hpp>

extern "C" void luabind_open(lua_State* L)
{
	luabind::open(L);
}
