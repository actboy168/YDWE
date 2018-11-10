#include <lua.hpp>
#include <base/hook/inline.h>
#include <base/hook/iat.h>
#include <bee/utility/unicode.h>	

static std::wstring lua_towstring(lua_State* L, int idx)
{
	size_t len = 0;
	const char* buf = luaL_checklstring(L, idx, &len);
	return bee::u2w(std::string_view(buf, len));
}

struct hook_t {
	enum class type {
		Inline,
		IAT,
	};
	type type = type::Inline;
	uintptr_t real = 0;
	uintptr_t fake = 0;
	base::hook::hook_t hi = 0;
};

static int push_funcdef(lua_State* L, int idx) {
	switch (lua_type(L, idx)) {
		case LUA_TSTRING:
			lua_pushvalue(L, idx);
			return 1;
		case LUA_TFUNCTION: {
			if (!lua_getupvalue(L, idx, 1)) {
				return luaL_argerror(L, idx, "has no upvalue");
			}
			if (lua_type(L, -1) != LUA_TUSERDATA) {
				return luaL_argerror(L, idx, lua_pushfstring(L, "bad upvalue #1, userdata expected, got %s", luaL_typename(L, -1)));
			}
			size_t fdef_len = 0;
			const char* fdef = luaL_tolstring(L, -1, &fdef_len);
			if (!fdef || fdef_len <= 7) {
				return luaL_argerror(L, idx, lua_pushfstring(L, "bad upvalue #1, userdata expected, got %s", luaL_typename(L, -1)));
			}
			lua_pushlstring(L, fdef + 6, fdef_len - 7);
			lua_remove(L, -2);
			return 1;
		}
		case LUA_TUSERDATA: {
			size_t fdef_len = 0;
			const char* fdef = luaL_tolstring(L, idx, &fdef_len);
			if (!fdef || fdef_len <= 7) {
				return luaL_argerror(L, idx, lua_pushfstring(L, "bad userdata #%d, can't get define", idx));
			}
			lua_pushlstring(L, fdef + 6, fdef_len - 7);
			return 1;
		}
		default:
			return luaL_argerror(L, idx, lua_pushfstring(L, "bad function def #%d, got %s", idx, luaL_typename(L, -1)));
	}
}

static int hook_call(lua_State* L) {
	luaL_testudata(L, 1, "hook");
	if (LUA_TTABLE != lua_getuservalue(L, 1)) {
		return 0;
	}
	if (LUA_TUSERDATA != lua_geti(L, -1, 1)) {
		return 0;
	}
	lua_remove(L, -2);
	lua_replace(L, 1);
	lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
	return lua_gettop(L);
}

static int hook_remove(lua_State* L) {
	hook_t* h = (hook_t*)luaL_testudata(L, 1, "hook");
	if (!h) {
		return 0;
	}
	switch (h->type) {
	case hook_t::type::Inline:
		base::hook::uninstall(&h->hi);
		break;
	}
	return 0;
}

int FFI_NEW;

static int hookInitialize(lua_State* L) {
	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_settop(L, 1);
	lua_rawsetp(L, lua_upvalueindex(1), &FFI_NEW);
	return 0;
}

static void infInline(lua_State* L, hook_t* h) {
	h->type = hook_t::type::Inline;
	lua_getupvalue(L, 1, 1);
	h->real = *(uintptr_t*)((uintptr_t)lua_touserdata(L, -1) + 16);
	lua_pop(L, 1);
	base::hook::install(&h->real, h->fake, &h->hi);
}

static void infIAT(lua_State* L, hook_t* h) {
	h->type = hook_t::type::IAT;
	const char* dllname = luaL_checkstring(L, 3);
	const char* apiname = nullptr;
	if (lua_isinteger(L, 4)) {
		apiname = (const char*)luaL_checkinteger(L, 4);
	}
	else {
		apiname = luaL_checkstring(L, 4);
	}
	if (lua_isinteger(L, 2)) {
		h->real = base::hook::iat((HMODULE)luaL_checkinteger(L, 2), dllname, apiname, h->fake);
	}
	else {
		std::wstring modulename = lua_towstring(L, 2);
		h->real = base::hook::iat(modulename.c_str(), dllname, apiname, h->fake);
	}
}

typedef void(*infHook)(lua_State* L, hook_t* h);

static int hookInterface(lua_State* L, infHook ih) {
	lua_rawgetp(L, lua_upvalueindex(1), &FFI_NEW);
	if (lua_type(L, -1) != LUA_TFUNCTION) {
		return luaL_error(L, "No initialization.");
	}

	lua_pushvalue(L, -1);
	lua_pushvalue(L, -4);
	lua_pushnil(L);
	lua_call(L, 2, 1);

	lua_insert(L, -4);
	lua_insert(L, -3);
	lua_call(L, 2, 1);

	hook_t* h = (hook_t*)lua_newuserdata(L, sizeof hook_t);
	if (!h) {
		return 0;
	}
	new (h) hook_t;
	luaL_setmetatable(L, "hook");
	lua_newtable(L);
	lua_pushvalue(L, -4);
	lua_seti(L, -2, 1);
	lua_pushvalue(L, -3);
	lua_seti(L, -2, 2);
	lua_setuservalue(L, -2);

	h->fake = *(uintptr_t*)((uintptr_t)lua_touserdata(L, -2) + 16);
	ih(L, h);
	uintptr_t cd = (uintptr_t)lua_touserdata(L, -3);
	*(uintptr_t*)(cd + 16) = h->real;
	return 1;
}

static int hookInline(lua_State* L) {
	luaL_checktype(L, 1, LUA_TFUNCTION);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	lua_settop(L, 2);
	push_funcdef(L, 1);
	lua_pushvalue(L, 2);
	return hookInterface(L, infInline);
}

static int hookIAT(lua_State* L) {
	luaL_checktype(L, 5, LUA_TFUNCTION);
	lua_settop(L, 5);
	push_funcdef(L, 1);
	lua_pushvalue(L, 5);
	return hookInterface(L, infIAT);
}

static int hookC2lua(lua_State* L) {
	luaL_checktype(L, 1, LUA_TSTRING);
	luaL_checktype(L, 2, LUA_TNUMBER);
	lua_rawgetp(L, lua_upvalueindex(1), &FFI_NEW);
	if (lua_type(L, -1) != LUA_TFUNCTION) {
		return luaL_error(L, "No initialization.");
	}
	uintptr_t fp = (uintptr_t)luaL_checkinteger(L, 2);
	lua_pushvalue(L, 1);
	lua_pushnil(L);
	lua_call(L, 2, 1);
	uintptr_t cd = (uintptr_t)lua_touserdata(L, -1);
	*(uintptr_t*)(cd + 16) = fp;
	return 1;
}

static int hookLua2c(lua_State* L) {
	luaL_checktype(L, 1, LUA_TSTRING);
	lua_rawgetp(L, lua_upvalueindex(1), &FFI_NEW);
	if (lua_type(L, -1) != LUA_TFUNCTION) {
		return luaL_error(L, "No initialization.");
	}
	lua_pushvalue(L, 1);
	lua_pushvalue(L, 2);
	lua_call(L, 2, 1);
	uintptr_t cd = (uintptr_t)lua_touserdata(L, -1);
	lua_pushinteger(L, *(uintptr_t*)(cd + 16));
	return 1;
}

extern "C" __declspec(dllexport)
int luaopen_hook(lua_State* L)
{
	static luaL_Reg mt[] = {
		{ "remove", hook_remove },
		{ "__call", hook_call },
		{ NULL, NULL },
	};
	luaL_newmetatable(L, "hook");
	luaL_setfuncs(L, mt, 0);

	static luaL_Reg lib[] = {
		{ "initialize", hookInitialize },
		{ "inline", hookInline },
		{ "iat", hookIAT },
		{ "c2lua", hookC2lua },
		{ "lua2c", hookLua2c },
		{ NULL, NULL }
	};
	luaL_newlibtable(L, lib);
	lua_pushvalue(L, -1);
	luaL_setfuncs(L, lib, 1);
	return 1;
}

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}
	return TRUE;
}
