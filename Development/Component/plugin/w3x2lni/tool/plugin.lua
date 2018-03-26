local lni = require 'lni'
local sandbox = require 'tool.sandbox'

local w2l
local config

local function load_one_plugin(path)
    local info = lni(io.load(path / 'info.ini'))
    local plugin = {
        name = info.info.name,
        version = info.info.version,
        author = info.info.author,
        description = info.info.description,
        enable = info.config.enable,
        path = path,
    }
    if not plugin.name then
        return
    end
    return plugin
end

local function load_plugins()
    local plugins = {}
    local plugin_path = fs.current_path() / config.plugin_path
    for path in plugin_path:list_directory() do
        if fs.is_directory(path) then
            local ok, res = pcall(load_one_plugin, path)
            if ok then
                plugins[#plugins+1] = res
            end
        end
    end
    table.sort(plugins, function (a, b)
        return a.name < b.name
    end)
    return plugins
end

local function call_plugin(plugin)
    local ok, err = pcall(sandbox, plugin.path:string()..'\\', io.open, {
        ['w3x2lni'] = w2l,
    })
    if not ok then
        w2l.message('-report|2警告', ('插件[%s]执行失败'):format(plugin.name))
        w2l.message('-tip', err)
    end
end

return function (w2l_, config_)
    w2l = w2l_
    config = config_
    if not config.plugin_path then
        return
    end
    local plugins = load_plugins()
    for _, plugin in ipairs(plugins) do
        if plugin.enable then
            call_plugin(plugin)
        end
    end
end
