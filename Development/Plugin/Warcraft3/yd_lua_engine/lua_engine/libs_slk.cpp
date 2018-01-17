#include <lua.hpp>
#include <Windows.h>
#include <base/path/helper.h>
#include <base/util/console.h>

namespace base { namespace warcraft3 { namespace lua_engine { namespace slk {

static const char sandbox[] = R"=(
local function standard(loaded)
    local r = {}
    for _, s in ipairs {
        --'package',
        'coroutine',
        'table',
        --'io',
        'os',
        'string',
        'math',
        'utf8',
        'debug',
    } do
        r[s] = _G[s]
        loaded[s] = _G[s]
    end
    for _, s in ipairs {
        'assert',
        'collectgarbage',
        --'dofile',
        'error',
        'getmetatable',
        'ipairs',
        --'loadfile',
        'load',
        'next',
        'pairs',
        'pcall',
        'print',
        'rawequal',
        'rawlen',
        'rawget',
        'rawset',
        'select',
        'setmetatable',
        'tonumber',
        'tostring',
        'type',
        'xpcall',
        '_VERSION',
        --'require',
    } do
        r[s] = _G[s]
    end
    return r
end

local function sandbox_env(root, loaded)
    local _LOADED = loaded or {}
    local _E = standard(_LOADED)
    local _ROOT = root
    local _PRELOAD = {}

    local rioOpen = loaded.io and loaded.io.open or io.open

    local function ioOpen(path, mode)
        return rioOpen(_ROOT .. path, mode)
    end

    _E.io = {
        open = ioOpen,
    }

    local function searchpath(name, path)
        local err = ''
    	name = string.gsub(name, '%.', '/')
    	for c in string.gmatch(path, '[^;]+') do
    		local filename = string.gsub(c, '%?', name)
    		local f = ioOpen(filename)
            if f then
                f:close()
    			return filename
    		end
            err = err .. ("\n\tno file '%s'"):format(filename)
        end
        return nil, err
    end

    local function loadfile(filename)
        local f, e = ioOpen(filename)
        if not f then
            return nil, e
        end
        local buf = f:read 'a'
        f:close()
        return load(buf, '@' .. _ROOT .. filename)
    end

    local function searcher_preload(name)
        assert(type(_PRELOAD) == "table", "'package.preload' must be a table")
        if _PRELOAD[name] == nil then
            return ("\n\tno field package.preload['%s']"):format(name)
        end
        return _PRELOAD[name]
    end
    
    local function searcher_lua(name)
        assert(type(_E.package.path) == "string", "'package.path' must be a string")
    	local filename, err = searchpath(name, _E.package.path)
    	if not filename then
    		return err
    	end
    	local f, err = loadfile(filename)
    	if not f then
    		error(("error loading module '%s' from file '%s':\n\t%s"):format(name, filename, err))
    	end
    	return f, filename
    end

    local function require_load(name)
        local msg = ''
        local _SEARCHERS = _E.package.searchers
        assert(type(_SEARCHERS) == "table", "'package.searchers' must be a table")
    	for _, searcher in ipairs(_SEARCHERS) do
            local f, extra = searcher(name)
            if type(f) == 'function' then
                return f, extra
            elseif type(f) == 'string' then
                msg = msg .. f
            end
        end
        error(("module '%s' not found:%s"):format(name, msg))
    end

    _E.require = function(name)
        assert(type(name) == "string", ("bad argument #1 to 'require' (string expected, got %s)"):format(type(name)))
    	local p = _LOADED[name]
    	if p ~= nil then
    		return p
    	end
    	local init, extra = require_load(name)
        debug.setupvalue(init, 1, _E)
    	local res = init(name, extra)
    	if res ~= nil then
    		_LOADED[name] = res
    	end
    	if _LOADED[name] == nil then
    		_LOADED[name] = true
    	end
    	return _LOADED[name]
    end
    _E.package = {
        config = [[
            \
            ;
            ?
            !
            -
        ]],
        loaded = _LOADED,
        path = '?.lua',
        preload = _PRELOAD,
        searchers = { searcher_preload, searcher_lua },
        searchpath = searchpath
    }
    return _E
end

local function getparent(path)
    if path then
        local pos = path:find [[[/\][^\/]*$]]
        if pos then
            return path:sub(1, pos)
        end
    end
end

return function(name, loadlua, loaded)
    local init, extra = loadlua(name)
    if not init then
        return error(("module '%s' not found"):format(name))
    end
    local root = getparent(extra)
    if not root then
        return error(("module '%s' not found"):format(name))
    end
    debug.setupvalue(init, 1, sandbox_env(root, loaded))
	return init(name, extra)
end
)=";
static const char slk[] = R"=(
local sandbox, loadlib, loadlua = ...
local w3x2lni = sandbox('w3x2lni', loadlua, {
    ['w3xparser'] = (loadlib 'w3xparser')(),
    ['lni-c']     = (loadlib 'lni-c')(),
    ['lpeg']      = (loadlib 'lpeg')(),
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
function w2l:mpq_load(filename)
	return load_mpq(filename)
end
function w2l:map_load(filename)
	return load_mpq(filename)
end
return w2l:slk_lib()
)=";

static int loadlib(lua_State* L)
{
	try {
		size_t len = 0;
		const char* str = luaL_checklstring(L, 1, &len);
		std::string name(str, len);
		fs::path path = path::ydwe(false) / "bin" / "modules" / (name + ".dll");
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

static bool loadlua_(lua_State* L, const fs::path& path)
{
	if (!fs::exists(path)) {
		return false;
	}
	const std::string filename = path.string();
	if (LUA_OK == luaL_loadfile(L, filename.c_str())) {
		lua_pushstring(L, filename.c_str());
		return true;
	}
	return true;
}

static int loadlua(lua_State* L)
{
	try {
		size_t len = 0;
		const char* str = luaL_checklstring(L, 1, &len);
		std::string name(str, len);
		if (loadlua_(L, path::ydwe(true) / "plugin" / name / "init.lua")) {
			return 2;
		}
		if (loadlua_(L, path::ydwe(true) / "plugin" / name / "init.lua")) {
			return 2;
		}
	}
	catch (...) {
	}
	return 0;
}

#define DoString(L, s, n) \
	(luaL_loadbuffer(L, s, sizeof(s) - 1, s) || (lua_insert(L, -(n)-1), lua_pcall(L, n, LUA_MULTRET, 0)))

int open(lua_State* L)
{
	if (DoString(L, sandbox, 0)) {
		printf("%s\n", lua_tostring(L, -1));
		return 0;
	}
	lua_pushcfunction(L, loadlib);
	lua_pushcfunction(L, loadlua);
	if (DoString(L, slk, 3)) {
		printf("%s\n", lua_tostring(L, -1));
		return 0;
	}
	return 1;
}
}}}}
