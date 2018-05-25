local lang = require 'share.lang'
local root = fs.current_path():parent_path()

local function load_plugins(source, callback, loadfile)
    local plugins = {}
    local config = loadfile '.config'
    if config then
        for name in config:gmatch '[^\r\n]+' do
            local ok, res = pcall(function()
                local buf, path = loadfile(name .. '.lua')
                return assert(load(buf, path and ('@'..path) or buf, 't', _ENV))()
            end)
            if ok then
                plugins[#plugins+1] = res
            end
        end
    end
    table.sort(plugins, function (a, b)
        return a.info.name < b.info.name
    end)
    for _, plugin in ipairs(plugins) do
        callback(source, plugin)
    end
end

return function (w2l, callback)
    load_plugins(lang.report.NATIVE, callback, function (name)
        local path = root / 'plugin' / name
        return io.load(path), path:string()
    end)
    load_plugins(lang.report.MAP, callback, function (name)
        return w2l:file_load('w3x2lni', 'plugin\\' .. name)
    end)
end
