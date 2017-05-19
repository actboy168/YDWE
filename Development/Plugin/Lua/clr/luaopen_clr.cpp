#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)
#include <base/filesystem.h>
#include <base/util/unicode.h>
#include <windows.h>
#include "clr_interop/clr_interop.h"

namespace clr {

	std::unique_ptr<appdomain> appdomain_;

	std::wstring luaL_checkwstring(lua_State* L, int idx)
	{
		size_t len = 0;
		const char* str = luaL_checklstring(L, idx, &len);
		return base::u2w(std::string_view(str, len), base::conv_method::replace | '?');
	}

	clr::object& to(lua_State* L, int idx)
	{
		return *(clr::object*)luaL_checkudata(L, idx, "clr-object");
	}

	int constructor(lua_State* L)
	{
		fs::path& assembly = *(fs::path*)luaL_checkudata(L, 1, "filesystem");
		std::wstring type = luaL_checkwstring(L, 2);
		void* storage = lua_newuserdata(L, sizeof(clr::object));
		luaL_getmetatable(L, "clr-object");
		lua_setmetatable(L, -2);
		new (storage)clr::object(appdomain_.get(), assembly.c_str(), type.c_str());
		return 1;
	}

	static int destructor(lua_State* L)
	{
		clr::object& self = to(L, 1);
		self.~object();
		return 0;
	}

	int call(lua_State* L)
	{
		clr::object& self = to(L, 1);
		std::wstring name = luaL_checkwstring(L, 2);
		lua_pushinteger(L, (lua_Integer)self.call(name.c_str()));
		return 1;
	}

	int error_code(lua_State* L)
	{
		clr::object& self = to(L, 1);
		lua_pushinteger(L, (lua_Integer)self.error_code());
		return 1;
	}
}

int luaopen_clr(lua_State* L)
{
	clr::appdomain_.reset(new clr::appdomain(nullptr));
	if (!*clr::appdomain_) {
		return luaL_error(L, "init clr failded.");
	}

	static luaL_Reg mt[] = {
		{ "call", clr::call },
		{ "error_code", clr::error_code },
		{ "__gc", clr::destructor },
		{ NULL, NULL }
	};
	luaL_newmetatable(L, "clr-object");
	luaL_setfuncs(L, mt, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	static luaL_Reg l[] = {
		{ "object", clr::constructor },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	lua_setglobal(L, "clr");
	return 0;
}
