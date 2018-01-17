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

local function sandbox_env(loadlua, openfile, loaded)
    local _LOADED = loaded or {}
    local _E = standard(_LOADED)
    local _PRELOAD = {}

    _E.io = {
        open = openfile,
    }

    local function searchpath(name, path)
        local err = ''
    	name = string.gsub(name, '%.', '/')
    	for c in string.gmatch(path, '[^;]+') do
            local filename = string.gsub(c, '%?', name)
            local f = openfile(filename)
            if f then
                f:close()
    			return filename
    		end
            err = err .. ("\n\tno file '%s'"):format(filename)
        end
        return nil, err
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
    	local f, err = loadlua(filename)
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

local function loadinit(name, read)
    local f = io._open(name, 'r')
    if not read then
        local ok = not not f
        f:close()
        return ok
    end
    if f then
        local str = f:read 'a'
        f:close()
        return load(str, '@' .. name)
    end
end

return function(root, io_open, loaded)
    local function openfile(name, mode)
        return io_open(root .. name, mode)
    end
    local function loadlua(name)
        local f = openfile(name, 'r')
        if f then
            local str = f:read 'a'
            f:close()
            return load(str, '@' .. root .. name)
        end
    end
    local init = loadlua('init.lua')
    debug.setupvalue(init, 1, sandbox_env(loadlua, openfile, loaded))
	return init()
end
)=";
static const char slk[] = R"=(
local sandbox, root, loadlib, io_open = ...
local w3x2lni = sandbox(root, io_open, {
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
return w2l:slk_lib(true, false)
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
	p->f = fopen(filename, mode);
	return (p->f == NULL) ? luaL_fileresult(L, 0, filename) : 1;
}

#define DoString(L, s, n) \
	(luaL_loadbuffer(L, s, sizeof(s) - 1, "module '" #s "'") || (lua_insert(L, -(n)-1), lua_pcall(L, n, LUA_MULTRET, 0)))

int open(lua_State* L)
{
	if (DoString(L, sandbox, 0)) {
		printf("%s\n", lua_tostring(L, -1));
		return 0;
	}
	fs::path root = base::path::ydwe(true) / "plugin" / "w3x2lni";
	lua_pushstring(L, (root.string() + "\\").c_str());
	lua_pushcfunction(L, loadlib);
	lua_pushcfunction(L, io_open);
	if (DoString(L, slk, 4)) {
		printf("%s\n", lua_tostring(L, -1));
		return 0;
	}
	return 1;
}
}}}}
