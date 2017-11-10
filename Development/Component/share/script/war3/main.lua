require 'utiliy'
require 'config'
require 'ffi.loadlibrary'
local ffi = require 'ffi'
local hk = require 'hook'
local event = require 'ev'
local storm = require 'ffi.storm'
hk.initialize(ffi.new)

local function sandbox_iat(define, module, dll, api, f)
    local h
    h = hk.iat(define, module, dll, api, function(...)
        local ok, e = xpcall(f, debug.traceback, h, ...)
        if ok then
            return e
        end
        log.error(e)
        return h(...)
    end)
    return h
end

local _, patch = global_config_war3_version()

sandbox_iat('void*(__stdcall*)(const char*)', 'War3.exe', 'kernel32.dll', 'LoadLibraryA', function (rLoadLibraryA, dllname)
    dllname = ffi.string(dllname)
    log.info('LoadLibraryA', dllname)
    if dllname:lower() == 'game.dll' then
        local lib
        if patch then
            lib = sys.load_library(patch / 'game.dll')
            storm.open(patch / 'patch.mpq', 9)
        end
        if not lib then
            lib = rLoadLibraryA(dllname)
        end
        event.emit('GameDll加载')
        return lib
    end
    return rLoadLibraryA(dllname)
end)

event.on('GameDll加载', function ()
    sys.load_library(fs.ydwe_path() / 'plugin' / 'warcraft3' / 'yd_loader.dll')
end)

event.on('GameDll加载', function ()
    local plugin = fs.ydwe_path() / "plugin" / "warcraft3"
    local cfg = sys.ini_load(plugin / 'config.cfg')
    local libs = {}
    for file in plugin:list_directory() do	
        if not fs.is_directory(file) 
        and file:filename():string():lower() ~= 'yd_loader.dll'
        and file:extension():string():lower() == '.dll'
        and cfg.Enable[file:filename():string()] ~= '0'
        then
            libs[#libs+1] = sys.load_library(file)
        end
    end
    local name, up = debug.getupvalue(ffi.C.GetProcAddress, 1)
    for _, lib in ipairs(libs) do
        local init = sys.get_proc_address(lib, 'Initialize', 'void(__stdcall*)()')
        if init then
            init()
        end
    end
end)
