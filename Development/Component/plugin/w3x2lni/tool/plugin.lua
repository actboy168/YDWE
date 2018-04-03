local lni = require 'lni'
local sandbox = require 'tool.sandbox'

local w2l
local config

local function load_one_plugin(load_in_disk, name)
    local buf, path = load_in_disk(name .. '.lua')
    local plugin = assert(load(buf, path and ('@'..path) or buf, 't', _ENV))()
    return {
        path = name,
        name = plugin.info and plugin.info.name or name,
        version = plugin.info and plugin.info.version or '未知',
        author = plugin.info and plugin.info.author or '未知',
        description = plugin.info and plugin.info.description,
        plugin = plugin,
    }
end

local function load_plugins(load_in_disk, list, source)
    local plugins = {}
    for name in pairs(list) do
        local ok, res = pcall(load_one_plugin, load_in_disk, name)
        if ok then
            plugins[#plugins+1] = res
            w2l.message('-report|9其他', ('使用的插件：[%s](%s)'):format(res.name, source))
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

local function load_enable_list(load_in_disk)
    local list = {}
    local buf = load_in_disk '.config'
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
    local ok, res = pcall(f, plugin.plugin, w2l)
    if ok then
        return res
    else
        w2l.message('-report|2警告', ('插件[%s]执行失败'):format(plugin.name))
        w2l.message('-tip', res)
    end
end

return function (w2l_, config_)
    w2l = w2l_
    config = config_
    if not config.plugin_path then
        return
    end

    local function load_in_disk(name)
        local path = fs.current_path() / config.plugin_path / name
        return io.load(path), path:string()
    end
    local enable_list = load_enable_list(load_in_disk)
    local plugins = load_plugins(load_in_disk, enable_list, '本地')

    local function load_in_map(name)
        return w2l:file_load('plugin', name)
    end
    local map_enable_list = load_enable_list(load_in_map)
    local map_plugins = load_plugins(load_in_map, map_enable_list, '地图')

    function w2l:call_plugin(event)
        for _, plugin in ipairs(plugins) do
            local res = call_plugin(plugin, event)
            if res ~= nil then
                return res
            end
        end
        for _, plugin in ipairs(map_plugins) do
            local res = call_plugin(plugin, event)
            if res ~= nil then
                return res
            end
        end
    end
end
