require 'filesystem'
require 'utility'
local root = fs.current_path():remove_filename()
local plugin_path = root / 'plugin'

local function load_one_plugin(path)
    local plugin = assert(load(io.load(path), '@'..path:string(), 't', _ENV))()
    return {
        path = path:stem():string(),
        name = plugin.info and plugin.info.name or path:stem():string(),
        version = plugin.info and plugin.info.version or '未知',
        author = plugin.info and plugin.info.author or '未知',
        description = plugin.info and plugin.info.description,
    }
end

local function load_plugins()
    local plugins = {}
    for path in plugin_path:list_directory() do
        if not fs.is_directory(path) and path:extension():string() == '.lua' then
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

local function load_enable_list()
    local list = {}
    local buf = io.load(plugin_path / '.config')
    if buf then
        for name in buf:gmatch '[^\r\n]+' do
            list[name] = true
        end
    end
    return list
end

local function save_enable_list(list)
    local array = {}
    for name, enable in pairs(list) do
        if enable then
            array[#array+1] = name
        end
    end
    table.sort(array)
    array[#array+1] = ''
    io.save(plugin_path / '.config', table.concat(array, '\r\n'))
end

local plugins
local last_clock
local current_plugin
local check_plugins
local enbale_list

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
                if checkbox_plugin(canvas, plugin.name, plugin, enable_list[plugin.path]) then
                    enable_list[plugin.path] = not enable_list[plugin.path]
                    save_enable_list(enable_list)
                end
            end
        end
    end)
    canvas:layout_row_dynamic(25, 1)
    if current_plugin then
        canvas:text('作者：' .. current_plugin.author, NK_TEXT_LEFT)
        canvas:text('版本：' .. current_plugin.version, NK_TEXT_LEFT)
    else
        canvas:text('', NK_TEXT_LEFT)
        canvas:text('', NK_TEXT_LEFT)
    end
end

function check_plugins()
    if not last_clock or os.clock() - last_clock > 1 then
        last_clock = os.clock()
        plugins = load_plugins()
        enable_list = load_enable_list()
    end
    if #plugins == 0 then
        return nil
    end
    return show_plugin
end

return check_plugins
