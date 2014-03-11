#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/warcraft3/virtual_mpq.h>

int luaopen_virtual_mpq(lua_State *pState)
{
	base::warcraft3::virtual_mpq::initialize(::GetModuleHandleW(NULL));

	using namespace luabind;
	module(pState, "virtual_mpq")
	[
		def("open_path", &base::warcraft3::virtual_mpq::open_path)
	];

	return 0;
}
