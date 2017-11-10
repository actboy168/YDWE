require 'utiliy'
require 'config'
require 'ffi.loadlibrary'
local ffi = require 'ffi'
local hk = require 'hook'
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
        sys.load_library(fs.ydwe_path() / 'plugin' / 'warcraft3' / 'yd_loader.dll')
        return lib
    end
    return rLoadLibraryA(dllname)
end)
