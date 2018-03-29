local lni = require 'lni'
local sandbox = require 'tool.sandbox'

local w2l
local config

local function load_one_plugin(path)
    local plugin = assert(load(io.load(path), '@'..path:string(), 't', _ENV))()
    return {
        name = path:stem():string(),
        version = plugin.info and plugin.info.version or '未知',
        author = plugin.info and plugin.info.author or '未知',
        description = plugin.info and plugin.info.description,
        plugin = plugin,
    }
end

local function load_plugins(list)
    local plugins = {}
    local plugin_path = fs.current_path() / config.plugin_path
    for name in pairs(list) do
        local path = plugin_path / (name .. '.lua')
        local ok, res = pcall(load_one_plugin, path)
        if ok then
            plugins[#plugins+1] = res
            w2l.message('-report|9其他', ('使用的插件：[%s]'):format(res.name))
            if res.description then
                w2l.message('-tip', res.description)
            end
        end
    end
    table.sort(plugins, function (a, b)
        return a.name < b.name
    end)
    return plugins
end

local function load_enable_list()
    local list = {}
    local buf = io.load(fs.current_path() / config.plugin_path / '.config')
    if buf then
        for name in buf:gmatch '[^\r\n]+' do
            list[name] = true
        end
    end
    return list
end

local function call_plugin(plugin, event)
    local f = plugin.plugin[event]
    if not f then
        return
    end
    local ok, err = pcall(f, plugin.plugin, w2l)
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
    local enable_list = load_enable_list()
    local plugins = load_plugins(enable_list)
    return function (event)
        for _, plugin in ipairs(plugins) do
            call_plugin(plugin, event)
        end
    end
end
