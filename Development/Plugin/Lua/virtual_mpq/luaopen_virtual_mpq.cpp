#include <lua.hpp>
#include <luabind/luabind.hpp>	   
#include <base/lua/luabind.h>
#include <base/warcraft3/virtual_mpq.h>	  
#include <LuaEngine/logging.h>

static bool VirtualMpqWatchCB(const luabind::object& func, const std::string& filename, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size)
{
	luabind::object ret;
	try {
		ret = luabind::call_function<luabind::object>(func, filename);
	} catch (luabind::error const& e) {
		const char* err = lua_tostring(e.state(), -1);
		LOGGING_ERROR(logging::get_logger("lua")) << err;
		return false;
	}

	switch (luabind::type(ret))
	{
	case LUA_TSTRING:
		break;
	case LUA_TNIL:
		return false;
	default:
		return false;
	}
	lua_State* L = ret.interpreter(); ret.push(L);
	size_t buflen = 0;
	const char* buf = lua_tolstring(L, -1, &buflen);
	void* tmpbuf = base::warcraft3::virtual_mpq::storm_alloc(buflen + reserve_size);
	if (!tmpbuf)
	{
		return false;
	}
	memcpy(tmpbuf, buf, buflen);
	*buffer_ptr = tmpbuf;
	if (reserve_size) memset((unsigned char*)tmpbuf + buflen, 0, reserve_size);
	if (size_ptr) *size_ptr = buflen;
	lua_pop(L, 1);
	return true;
}

static void VirtualMpqWatch(lua_State* L, const std::string& filename, const luabind::object& func)
{
	base::warcraft3::virtual_mpq::watch(filename, std::bind(VirtualMpqWatchCB, func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

int luaopen_virtual_mpq(lua_State *pState)
{
	base::warcraft3::virtual_mpq::initialize(::GetModuleHandleW(NULL));

	using namespace luabind;
	module(pState, "virtual_mpq")
	[
		def("open_path", &base::warcraft3::virtual_mpq::open_path),
		def("watch", &VirtualMpqWatch)
	];

	return 0;
}
