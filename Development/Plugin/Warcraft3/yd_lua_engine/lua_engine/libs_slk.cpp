#include <lua.hpp>
#include <Windows.h>
#include <base/path/ydwe.h>
#include <base/util/console.h>
#include <bee/utility/unicode_win.h>

namespace warcraft3::lua_engine::slk {

static const char slk[] = R"=(
local ydwePath, loadlua, loadlib, io_open = ...
local w2lPath = ('%s/plugin/w3x2lni'):format(ydwePath)
local sandbox = loadlua(('%s/script/backend/sandbox.lua'):format(w2lPath))
local slklib = loadlua(('%s/compiler/script/w3x2lni/slk_lib.lua'):format(ydwePath))
local function load_file(filename)
	local f = io_open(filename, "r")
	if f then
		local buf = f:read "a"
		f:close()
		return buf
	end
end
local function data_load(w2l, filename)
    if filename:sub(1, 3) == 'ui\\' then
		return load_file(('%s/%s'):format(ydwePath, filename))
    end
    if filename:sub(1, 9) == 'prebuilt\\' then
		return load_file(('%s/data/%s/%s'):format(w2lPath, w2l.setting.data, filename))
    end
	return load_file(('%s/share/zh-CN/%s'):format(ydwePath, filename))
end

local w3x2lni = sandbox(('%s/script/core/'):format(w2lPath), io_open, {
    ['w3xparser'] = (loadlib 'w3xparser')(),
    ['lni']       = (loadlib 'lni')(),
    ['lml']       = (loadlib 'lml')(),
    ['lpeglabel'] = (loadlib 'lpeglabel')(),
    ['data_load'] = data_load,
})
local function load_mpq(filename)
	local f = io.open(filename, "r")
	if f then
		local buf = f:read "a"
		f:close()
		return buf
	end
end
local w2l = w3x2lni()
w2l.input_ar = {}
function w2l.input_ar:get(filename)
	return load_mpq(filename)
end
return slklib(w2l, true, false)
)=";

static int loadlib(lua_State* L)
{
	try {
		size_t len = 0;
		const char* str = luaL_checklstring(L, 1, &len);
		std::string name(str, len);
		fs::path path = base::path::ydwe(false) / "bin" / (name + ".dll");
		HMODULE m = LoadLibraryW(path.c_str());
		if (!m) {
			return 0;
		}
		std::string funcname = "luaopen_" + name;
		size_t pos = funcname.find_last_of('-');
		if (pos != std::string::npos) {
			funcname = funcname.substr(0, pos);
		}
		lua_CFunction init = (lua_CFunction)GetProcAddress(m, funcname.c_str());
		if (!init) {
			return 0;
		}
		lua_pushcfunction(L, init);
		return 1;
	}
	catch (...) {
	}
	return 0;
}

static int loadlua(lua_State* L)
{
	size_t len = 0;
	const char* str = luaL_checklstring(L, 1, &len);
	std::string name(str, len);
	if (luaL_loadfile(L, name.c_str())) {
		return lua_error(L);
	}
	lua_call(L, 0, 1);
	return 1;
}

static int io_fclose(lua_State *L) {
	luaL_Stream *p = (luaL_Stream *)luaL_checkudata(L, 1, LUA_FILEHANDLE);
	int res = fclose(p->f);
	return luaL_fileresult(L, (res == 0), NULL);
}

static int io_open(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	const char *mode = luaL_optstring(L, 2, "r");
	luaL_argcheck(L, mode[0] == 'r' && mode[1] == '\0', 2, "invalid mode");
	luaL_Stream *p = (luaL_Stream *)lua_newuserdata(L, sizeof(luaL_Stream));
	luaL_setmetatable(L, LUA_FILEHANDLE);
	p->closef = &io_fclose;
	p->f = _wfopen(bee::u2w(filename).c_str(), L"r");
	return (p->f == NULL) ? luaL_fileresult(L, 0, filename) : 1;
}

#define DoString(L, s, n) \
	(luaL_loadbuffer(L, s, sizeof(s) - 1, "module '" #s "'") || (lua_insert(L, -(n)-1), lua_pcall(L, n, LUA_MULTRET, 0)))

#define DoFile(L, f, n) \
	(luaL_loadfile(L, f) || (lua_insert(L, -(n)-1), lua_pcall(L, n, LUA_MULTRET, 0)))

int open(lua_State* L)
{
	fs::path ydwe = base::path::ydwe(true);
	lua_pushstring(L, bee::w2u(ydwe.wstring()).c_str());
	lua_pushcfunction(L, loadlua);
	lua_pushcfunction(L, loadlib);
	lua_pushcfunction(L, io_open);
	if (DoString(L, slk, 4)) {
		printf("%s\n", lua_tostring(L, -1));
		return 0;
	}
	return 1;
}
}
