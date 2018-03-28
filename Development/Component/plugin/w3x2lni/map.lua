require 'filesystem'
require 'utility'
local lni = require 'lni'
local uni = require 'ffi.unicode'
local core = require 'tool.sandbox_core'
local builder = require 'map-builder'
local triggerdata = require 'tool.triggerdata'
local plugin = require 'tool.plugin'
local w2l = core()

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

local input = config.input
print('正在打开地图...')
local slk = {}
local input_ar = builder.load(input)
if not input_ar then
    os.exit(1, true)
    return
end

local output = config.output or default_output(config.input)
if w2l.config.target_storage == 'dir' then
    fs.create_directories(output)
end
local output_ar = builder.load(output, 'w')
if not output_ar then
    os.exit(1, true)
    return
end
output_ar:flush()

local function is_input_lni()
    if fs.is_directory(input) then
        local map = io.open((input / 'builder.w3x'):string(), 'rb')
        if map then
            map:seek('set', 8)
            local mark = map:read(4)
            if mark == 'W2L\x01' then
                return true
            end
        end
    end
    return false
end

if is_input_lni() then
    w2l.input_mode = 'lni'
end

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
    else
        if type == 'script' and not self.config.export_lua then
            return
        end
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
        if name:sub(-4) == '.mdx' or name:sub(-4) == '.mdl' or name:sub(-4) == '.blp' or name:sub(-4) == '.tga' then
            type = 'resource'
        elseif name:sub(-4) == '.lua' or name:sub(-4) == '.ini' then
            type = 'script'
        elseif name:sub(-4) == '.mp3' or name:sub(-4) == '.wav' then
            type = 'sound'
        else
            type = 'map'
        end
        if w2l.input_mode == 'lni' then
            name = name:sub(#type + 2)
        end
        return type, name, buf
    end
end

function w2l:mpq_load(filename)
    return w2l.mpq_path:each_path(function(path)
        return io.load(fs.current_path() / config.mpq_path / path / filename)
    end)
end

function w2l:prebuilt_load(filename)
    return w2l.mpq_path:each_path(function(path)
        return io.load(fs.current_path() / config.prebuilt_path / path / filename)
    end)
end

function w2l:trigger_data()
    return triggerdata()
end

local function save_builder(doo)
    if w2l.config.mode == 'lni' then
        local path = output / 'builder.w3x'
        local ex_map = builder.load(path, 'w')
        ex_map:set('war3mapunits.doo', w2l:create_unitsdoo())
        ex_map:set('war3map.doo', doo)
        ex_map:set('war3map.w3e', w2l:create_w3e())
        ex_map:set('war3map.w3i', w2l:backend_w3i(slk.w3i, slk.wts))
        slk.w3i['地图']['地图名称'] = 'W2L\x01'
        ex_map:save(slk.w3i, w2l)
        ex_map:close()
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

print('正在读取文件...')
w2l.progress:start(input_rate)
input_ar:search_files(w2l.progress)
w2l.progress:finish()

print('正在读取物编...')
w2l.progress:start(input_rate + frontend_rate)
w2l:frontend(slk)
w2l.progress:finish()

print('加载插件...')
plugin(w2l, config)

print('正在转换...')
w2l.progress:start(input_rate + frontend_rate + backend_rate)
w2l:backend(slk)
w2l.progress:finish()

print('正在生成文件...')
local doo = input_ar:get 'war3map.doo'
w2l.progress:start(1)
builder.save(w2l, output_ar, slk.w3i, input_ar)
w2l.progress:finish()

save_builder(doo)
print('转换完毕,用时 ' .. os.clock() .. ' 秒') 
