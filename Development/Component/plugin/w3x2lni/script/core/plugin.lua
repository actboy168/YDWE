local lang = require 'lang'

local function load_plugins(source, callback, loadfile)
    local plugins = {}
    local config = loadfile '.config'
    if config then
        for name in config:gmatch '[^\r\n]+' do
            local ok, res = pcall(function()
                local buf = loadfile(name .. '.lua')
                return assert(load(buf, buf, 't', _ENV))()
            end)
            if ok then
                plugins[#plugins+1] = res
            end
        end
    end
    for _, plugin in ipairs(plugins) do
        callback(source, plugin)
    end
end

return function (w2l, callback)
    load_plugins(lang.report.NATIVE, callback, function (name)
        return w2l:data_load('plugin/' .. name)
    end)
    load_plugins(lang.report.MAP, callback, function (name)
        return w2l:file_load('w3x2lni', 'plugin\\' .. name)
    end)
end
