
local ffi = require 'ffi'
local hk = require 'hook'

hk.initialize(ffi.new)

local hook = {}

function hook.iat(define, module, dll, api, f)
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

local dyn_h = {}

function hook.dyn_iat(define, module, dll, api, f)
    local list = dyn_h[module]
    if not list then
        list = {}
        list.rGetProcAddress = hk.iat('int(__stdcall*)(void*, const char*)', module, 'kernel32.dll', 'GetProcAddress', function(lib, name)
            name = ffi.string(name)
            for _, l in ipairs(list) do
                local dll, api, f = l[1], l[2], l[3]
                if lib == sys.get_module_handle(dll) and name == api then
                    local res = list.rGetProcAddress(lib, name)
                    l[4] = hk.c2lua(define, res)
                    return hk.lua2c(define, f)
                end
            end
            return list.rGetProcAddress(lib, name)
        end)
        dyn_h[module] = list
    end
    local l = {dll, api}
    l[3] = ffi.new(define, function(...)
        local ok, e = xpcall(f, debug.traceback, l[4], ...)
        if ok then
            return e
        end
        log.error(e)
        return l[4](...)
    end)
    list[#list+1] = l
end

return hook
