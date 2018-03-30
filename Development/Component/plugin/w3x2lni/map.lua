require 'filesystem'
require 'utility'
local lni = require 'lni'
local uni = require 'ffi.unicode'
local core = require 'tool.sandbox_core'
local builder = require 'map-builder'
local triggerdata = require 'tool.triggerdata'
local plugin = require 'tool.plugin'
local w2l = core()
local root = fs.current_path()

local std_print = print
function print(...)
    local tbl = {...}
    local count = select('#', ...)
    for i = 1, count do
        tbl[i] = tostring(tbl[i]):gsub('[\r\n]', ' ')
    end
    std_print(table.concat(tbl, ' '))
end
w2l:set_messager(print)

local function unpack_config()
    local config = {}
    for _, command in ipairs(arg) do
        if command:sub(1, 1) == '-' then
            if command == '-slk' then
                config.mode = 'slk'
            elseif command == '-lni' then
                config.mode = 'lni'
            elseif command == '-obj' then
                config.mode = 'obj'
            elseif command:match '^%-config=' then
                config.config_path = command:sub(1 + #'-config=')
            end
        else
            if not config.input then
                config.input = fs.path(command)
            else
                config.output = fs.path(command)
            end
        end
    end
    if not config.config_path or not config.mode then
        return config
    end
    local tbl = lni(io.load(fs.path(config.config_path)))
    for k, v in pairs(tbl) do
        config[k] = v
    end
    for k, v in pairs(tbl[config.mode]) do
        config[k] = v
    end
    return config
end

local function default_output(input)
    if w2l.config.target_storage == 'dir' then
        if fs.is_directory(input) then
            return input:parent_path() / (input:filename():string() .. '_' .. w2l.config.mode)
        else
            return input:parent_path() / input:stem():string()
        end
    elseif w2l.config.target_storage == 'mpq' then
        if fs.is_directory(input) then
            return input:parent_path() / (input:filename():string() .. '.w3x')
        else
            return input:parent_path() / (input:stem():string() .. '_' .. w2l.config.mode .. '.w3x')
        end
    end
end

local config = unpack_config()
w2l:set_config(config)
if config.mode == 'slk' then
    print('-title Slk优化')
end

input = config.input
local function check_lni_mark(path)
    local map = io.open(path:string(), 'rb')
    if map then
        map:seek('set', 8)
        local mark = map:read(4)
        if mark == 'W2L\x01' then
            return true
        end
    end
end

local function check_input_lni()
    if fs.is_directory(input) then
        if check_lni_mark(input / '.w3x') then
            return true
        end
    else
        if check_lni_mark(input) then
            input = input:parent_path()
            config.input = input
            return true
        end
    end
    return false
end

if check_input_lni() then
    w2l.input_mode = 'lni'
end

print('正在打开地图...')
local slk = {}
local input_ar = builder.load(input)
if not input_ar then
    os.exit(1, true)
    return
end

output = config.output or default_output(config.input)
if w2l.config.target_storage == 'dir' then
    if not fs.exists(output) then
        fs.create_directories(output)
    end
end
local output_ar = builder.load(output, 'w')
if not output_ar then
    os.exit(1, true)
    return
end
output_ar:flush()

function w2l:map_load(filename)
    return input_ar:get(filename)
end

function w2l:map_save(filename, buf)
    input_ar:set(filename, buf)
end

function w2l:map_remove(filename)
    input_ar:remove(filename)
end

function w2l:file_save(type, name, buf)
    if type == 'table' then
        input_ar:set(self.info.lni_dir[name][1], buf)
        output_ar:set(self.info.lni_dir[name][1], buf)
    elseif type == 'trigger' then
        input_ar:set('trigger/' .. name, buf)
        output_ar:set('trigger/' .. name, buf)
    elseif type == 'scripts' then
        if not self.config.export_lua then
            return
        end
        input_ar:set('scripts/' .. name, buf)
        output_ar:set('scripts/' .. name, buf)
    elseif type == 'plugin' then
        input_ar:set('plugin/' .. name, buf)
        output_ar:set('plugin/' .. name, buf)
    else
        if self.input_mode == 'lni' then
            input_ar:set(type .. '/' .. name, buf)
        else
            input_ar:set(name, buf)
        end
        if self.config.mode == 'lni' then
            output_ar:set(type .. '/' .. name, buf)
        else
            output_ar:set(name, buf)
        end
    end
end

function w2l:file_load(type, name)
    if type == 'table' then
        for _, filename in ipairs(self.info.lni_dir[name]) do
            local buf = input_ar:get(filename)
            if buf then
                return buf
            end
        end
    elseif type == 'trigger' then
        return input_ar:get('trigger/' .. name) or input_ar:get('war3map.wtg.lml/' .. name)
    elseif type == 'scripts' then
        return input_ar:get('scripts/' .. name)
    elseif type == 'plugin' then
        return input_ar:get('plugin/' .. name)
    else
        if self.input_mode == 'lni' then
            return input_ar:get(type .. '/' .. name)
        else
            return input_ar:get(name)
        end
    end
end

function w2l:file_remove(type, name)
    if type == 'table' then
        for _, filename in ipairs(self.info.lni_dir[name]) do
            input_ar:remove(filename)
            output_ar:remove(filename)
        end
    elseif type == 'trigger' then
        input_ar:remove('trigger/' .. name, buf)
        input_ar:remove('war3map.wtg.lml/' .. name, buf)
        output_ar:remove('trigger/' .. name, buf)
        output_ar:remove('war3map.wtg.lml/' .. name, buf)
    elseif type == 'scripts' then
        input_ar:remove('scripts/' .. name, buf)
        output_ar:remove('scripts/' .. name, buf)
    elseif type == 'plugin' then
        input_ar:remove('plugin/' .. name, buf)
        output_ar:remove('plugin/' .. name, buf)
    else
        if self.input_mode == 'lni' then
            input_ar:remove(type .. '/' .. name, buf)
        else
            input_ar:remove(name, buf)
        end
        if self.config.mode == 'lni' then
            output_ar:remove(type .. '/' .. name, buf)
        else
            output_ar:remove(name, buf)
        end
    end
end

function w2l:file_pairs()
    local next, tbl, index = input_ar:search_files()
    return function ()
        local name, buf = next(tbl, index)
        if not name then
            return nil
        end
        index = name
        local type
        local dir = name:match '^[^/\\]+' :lower()
        local ext = name:match '[^%.]+$'
        if ext == 'mdx' or ext == 'mdl' or ext == 'blp' or ext == 'tga' then
            type = 'resource'
        elseif ext == 'mp3' or ext == 'wav' then
            type = 'sound'
        elseif name == 'scripts\\war3map.j' then
            type = 'map'
        elseif dir == 'scripts' then
            type = 'scripts'
        elseif dir == 'plugin' then
            type = 'plugin'
        else
            type = 'map'
        end
        if w2l.input_mode == 'lni' or type == 'scripts' or type == 'plugin' then
            if dir == type then
                name = name:sub(#type + 2)
            end
        end
        return type, name, buf
    end
end

function w2l:mpq_load(filename)
    return w2l.mpq_path:each_path(function(path)
        return io.load(root / config.mpq_path / path / filename)
    end)
end

function w2l:prebuilt_load(filename)
    return w2l.mpq_path:each_path(function(path)
        return io.load(root / config.prebuilt_path / path / filename)
    end)
end

function w2l:trigger_data()
    return triggerdata()
end

local function save_builder()
    if w2l.config.mode == 'lni' then
        fs.copy_file(root / 'map-builder' / '.w3x', output / '.w3x', true)
    end
end

local file_count = input_ar:number_of_files()
local function get_io_time(map)
    local io_speed = map:get_type() == 'mpq' and 30000 or 10000
    local io_rate = math.min(0.3, file_count / io_speed)
    return io_rate
end
local input_rate = get_io_time(input_ar)
local output_rate = get_io_time(output_ar)
local frontend_rate = (1 - input_rate - output_rate) * 0.4
local backend_rate = (1 - input_rate - output_rate) * 0.6

print('正在检查插件...')
local call_plugin = plugin(w2l, config)

print('正在读取文件...')
w2l.progress:start(input_rate)
input_ar:search_files(w2l.progress)
w2l.progress:finish()

print('正在读取物编...')
w2l.progress:start(input_rate + frontend_rate)
w2l:frontend(slk)
w2l.progress:finish()

print('正在执行插件...')
call_plugin('on_complete_data')

print('正在转换...')
w2l.progress:start(input_rate + frontend_rate + backend_rate)
w2l:backend(slk)
w2l.progress:finish()

print('正在生成文件...')
local doo = input_ar:get 'war3map.doo'
w2l.progress:start(1)
builder.save(w2l, output_ar, slk.w3i, input_ar)
w2l.progress:finish()

save_builder()
print('转换完毕,用时 ' .. os.clock() .. ' 秒') 
