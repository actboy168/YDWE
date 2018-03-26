require 'filesystem'
require 'utility'
local lni = require 'lni'
local root = fs.current_path():remove_filename()
local plugin_path = root / 'plugin'

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

local info_buf = [[
[info]
name = $name$
version = $version$
author = $author$
description = $description$

[config]
enable = $enable$
]]
local function save_info(plugin)
    local buf = info_buf:gsub('%$(.-)%$', function (key)
        return tostring(plugin[key])
    end)
    buf = buf:gsub('\n', '\r\n')
    io.save(plugin.path / 'info.ini', buf)
end

local function load_plugins()
    local plugins = {}
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

local plugins
local last_clock
local current_plugin
local check_plugins

local function checkbox_plugin(canvas, text, plugin, active)
    canvas:layout_space_push(0, 0, 320, 25)
    local ok, _, state = canvas:checkbox(text, active)
    if state & NK_WIDGET_STATE_LEFT ~= 0 then
        current_plugin = plugin
    end
    if plugin.description then
        canvas:layout_space_push(0, 0, 320, 25)
        canvas:text('', NK_TEXT_LEFT, plugin.description)
    end
    return ok
end

local function show_plugin(canvas)
    check_plugins()
    current_plugin = nil
    canvas:layout_row_dynamic(415, 1)
    canvas:group('说明', function()
        if plugins then
            for _, plugin in ipairs(plugins) do
                canvas:layout_space(25, 2)
                if checkbox_plugin(canvas, plugin.name, plugin, plugin.enable) then
                    plugin.enable = not plugin.enable
                    save_info(plugin)
                end
            end
        end
    end)
    canvas:layout_row_dynamic(25, 1)
    if current_plugin then
        canvas:text('作者：' .. (current_plugin.author or '未知'), NK_TEXT_LEFT)
        canvas:text('版本：' .. (current_plugin.version or '未知'), NK_TEXT_LEFT)
    else
        canvas:text('', NK_TEXT_LEFT)
        canvas:text('', NK_TEXT_LEFT)
    end
end

function check_plugins()
    if not last_clock or os.clock() - last_clock > 1 then
        last_clock = os.clock()
        plugins = load_plugins()
    end
    if #plugins == 0 then
        return nil
    end
    return show_plugin
end

return check_plugins
