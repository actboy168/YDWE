local lni = require 'lni'
local sandbox = require 'backend.sandbox'
local lang = require 'share.lang'

local w2l
local config

local function load_one_plugin(load_in_disk, name)
    local buf, path = load_in_disk(name .. '.lua')
    local plugin = assert(load(buf, path and ('@'..path) or buf, 't', _ENV))()
    return {
        path = name,
        name = plugin.info and plugin.info.name or name,
        version = plugin.info and plugin.info.version or lang.script.UNKNOW,
        author = plugin.info and plugin.info.author or lang.script.UNKNOW,
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
            w2l.messager.report(lang.report.OTHER, 9, lang.report.USED_PLUGIN:format(res.name, source), res.description)
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
        w2l.messager.report(lang.report.OTHER, 2, lang.report.PLUGIN_FAILED:format(plugin.name), res)
    end
end

return function (w2l_, config_)
    w2l = w2l_
    config = config_

    local function load_in_disk(name)
        local path = fs.current_path():parent_path() / 'plugin' / name
        return io.load(path), path:string()
    end
    local enable_list = load_enable_list(load_in_disk)
    local plugins = load_plugins(load_in_disk, enable_list, lang.report.NATIVE)

    local function load_in_map(name)
        return w2l:file_load('w3x2lni', 'plugin\\' .. name)
    end
    local map_enable_list = load_enable_list(load_in_map)
    local map_plugins = load_plugins(load_in_map, map_enable_list, lang.report.MAP)

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
