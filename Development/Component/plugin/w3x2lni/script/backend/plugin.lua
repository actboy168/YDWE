local lang = require 'share.lang'
local base = require 'backend.base_path'
local messager = require 'share.messager'

local function load_plugins(source, callback, loadfile)
    local plugins = {}
    local config = loadfile '.config'
    if config then
        for name in config:gmatch '[^\r\n]+' do
            local ok, res = xpcall(function()
                local buf = loadfile(name .. '.lua')
                return assert(load(buf, ('@plugin\\'..name..'.lua'), 't', _ENV))()
            end, debug.traceback)
            if ok then
                if not res.info then
                    res.info = {}
                end
                res.info.name        = res.info.name        or ('<%s>'):format(name)
                res.info.description = res.info.description or ""
                res.info.author      = res.info.author      or "<Unknown>"
                res.info.version     = res.info.version     or "<Unknown>"
                plugins[#plugins+1]  = res
            else
                messager.report(lang.report.WARN, 2, lang.report.PLUGIN_FAILED:format(name), res)
            end
        end
    end
    for _, plugin in ipairs(plugins) do
        callback(source, plugin)
    end
end

return function (w2l, callback)
    load_plugins(lang.report.NATIVE, callback, function (name)
        return io.load(base / 'plugin' / name)
    end)
    load_plugins(lang.report.MAP, callback, function (name)
        return w2l:file_load('w3x2lni', 'plugin\\' .. name)
    end)
end
