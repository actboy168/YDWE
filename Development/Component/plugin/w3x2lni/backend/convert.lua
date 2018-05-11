local messager = require 'tool.messager'
local core = require 'backend.sandbox_core'
local builder = require 'map-builder'
local triggerdata = require 'backend.triggerdata'
local plugin = require 'tool.plugin'
local lang = require 'tool.lang'
local get_report = require 'tool.report'
local root_path = require 'backend.root_path'
local check_lni_mark = require 'tool.check_lni_mark'
local unpack_config = require 'backend.unpack_config'
local w2l = core()
local root = fs.current_path()
local config
local input_ar
local output_ar

local report = {}
local messager_report = messager.report
function messager.report(type, level, content, tip)
    messager_report(type, level, content, tip)
    local name = level .. type
    if not report[name] then
        report[name] = {}
    end
    table.insert(report[name], {content, tip})
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

local function check_input_lni()
    if fs.is_directory(input) and check_lni_mark(input / '.w3x') then
        return true
    end
    return false
end

local function exit(report)
    local err = 0
    local warn = 0
    for k, t in pairs(report) do
        if k:sub(1, 1) == '1' then
            err = #t
        elseif k:sub(1, 1) == '2' then
            warn = #t
        end
    end
    if err > 0 then
        messager.exit('error', lang.script.ERROR_COUNT:format(err, warn))
    elseif warn > 0 then
        messager.exit('warning', lang.script.ERROR_COUNT:format(err, warn))
    else
        messager.exit('success', lang.script.ERROR_COUNT:format(err, warn))
    end
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
    elseif type == 'scripts' then
        if not self.config.export_lua then
            return
        end
        input_ar:set('scripts/' .. name, buf)
        output_ar:set('scripts/' .. name, buf)
    elseif type == 'w3x2lni' then
        input_ar:set('w3x2lni/' .. name, buf)
        output_ar:set('w3x2lni/' .. name, buf)
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
    elseif type == 'w3x2lni' then
        return input_ar:get('w3x2lni/' .. name)
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
    elseif type == 'w3x2lni' then
        input_ar:remove('w3x2lni/' .. name, buf)
        output_ar:remove('w3x2lni/' .. name, buf)
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
        elseif dir == 'w3x2lni' then
            type = 'w3x2lni'
        else
            type = 'map'
        end
        if w2l.input_mode == 'lni' or type == 'scripts' or type == 'w3x2lni' then
            if dir == type then
                name = name:sub(#type + 2)
            end
        end
        return type, name, buf
    end
end

function w2l:mpq_load(filename)
    return w2l.mpq_path:each_path(function(path)
        return io.load(root:parent_path() / 'data' / self.config.data_war3 / 'war3' / path / filename)
    end)
end

function w2l:prebuilt_load(filename)
    return w2l.mpq_path:each_path(function(path)
        return io.load(root:parent_path() / 'data' / self.config.data_war3 / 'prebuilt' / path / filename)
    end)
end

function w2l:meta_load(filename)
    return io.load(root:parent_path() / 'data' / self.config.data_meta / 'we' / filename)
end

function w2l:wes_load(filename)
    return io.load(root:parent_path() / 'data' / self.config.data_wes / 'we' / filename)
end

function w2l:trigger_data()
    return triggerdata(self.config.data_ui)
end

local function save_builder()
    if w2l.config.mode == 'lni' then
        fs.copy_file(root / 'map-builder' / '.w3x', output / '.w3x', true)
    end
end

local function get_io_time(map, file_count)
    local io_speed = map:get_type() == 'mpq' and 30000 or 10000
    local io_rate = math.min(0.3, file_count / io_speed)
    return io_rate
end

return function (mode)
    config = unpack_config(mode)
    input = config.input

    w2l:set_messager(messager)
    if not input then
        w2l:failed(lang.script.NO_INPUT)
    end

    w2l.messager.text(lang.script.INIT)
    w2l.messager.progress(0)

    if check_input_lni() then
        w2l.input_mode = 'lni'
    end

    if config.mode == 'slk' then
        messager.title 'Slk'
    elseif config.mode == 'obj' then
        messager.title 'Obj'
    elseif config.mode == 'lni' then
        messager.title 'Lni'
    end

    messager.text(lang.script.OPEN_MAP)
    local err
    input_ar, err = builder.load(input)
    if not input_ar then
        w2l:failed(err)
    end
    w2l:set_config(config)
    
    output = config.output or default_output(config.input)
    if w2l.config.target_storage == 'dir' then
        if not fs.exists(output) then
            fs.create_directories(output)
        end
    end
    output_ar, err = builder.load(output, 'w')
    if not output_ar then
        w2l:failed(err)
    end
    output_ar:flush()

    local slk = {}
    local file_count = input_ar:number_of_files()
    local input_rate = get_io_time(input_ar, file_count)
    local output_rate = get_io_time(output_ar, file_count)
    local frontend_rate = (1 - input_rate - output_rate) * 0.4
    local backend_rate = (1 - input_rate - output_rate) * 0.6

    messager.text(lang.script.CHECK_PLUGIN)
    plugin(w2l, config)
    
    w2l:call_plugin('on_convert')
    
    messager.text(lang.script.LOAD_FILE)
    w2l.progress:start(input_rate)
    input_ar:search_files(w2l.progress)
    w2l.progress:finish()

    if w2l:file_load('w3x2lni', 'locale/w3i.lng') then
        lang:set_lng_file('w3i', w2l:file_load('w3x2lni', 'locale/w3i.lng'))
    end
    if w2l:file_load('w3x2lni', 'locale/lml.lng') then
        lang:set_lng_file('lml', w2l:file_load('w3x2lni', 'locale/lml.lng'))
    end
    
    messager.text(lang.script.LOAD_OBJECT)
    w2l.progress:start(input_rate + frontend_rate)
    w2l:frontend(slk)
    w2l.progress:finish()
    
    messager.text(lang.script.DO_PLUGIN)
    w2l:call_plugin('on_full')
    
    messager.text(lang.script.DO_CONVERT)
    w2l.progress:start(input_rate + frontend_rate + backend_rate)
    w2l:backend(slk)
    w2l.progress:finish()

    if w2l.config.mode == 'lni' then
        local path = root / 'locale' / lang:current_lang() / 'w3i.lng'
        w2l:file_save('w3x2lni', 'locale/w3i.lng', io.load(path) or '')
        local path = root / 'locale' / lang:current_lang() / 'lml.lng'
        w2l:file_save('w3x2lni', 'locale/lml.lng', io.load(path) or '')
    else
        w2l:file_remove('w3x2lni', 'locale/w3i.lng')
        w2l:file_remove('w3x2lni', 'locale/lml.lng')
    end
    
    messager.text(lang.script.SAVE_FILE)
    local doo = input_ar:get 'war3map.doo'
    w2l.progress:start(1)
    builder.save(w2l, output_ar, slk.w3i, input_ar)
    w2l.progress:finish()
    
    save_builder()
    fs.create_directories(root:parent_path() / 'log')
    io.save(root:parent_path() / 'log' / 'report.log', get_report(report))
    messager.text((lang.script.FINISH):format(os.clock()))
    exit(report)
end
