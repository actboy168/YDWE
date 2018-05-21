local messager = require 'share.messager'
local core = require 'backend.sandbox_core'
local builder = require 'map-builder'
local triggerdata = require 'backend.triggerdata'
local plugin = require 'share.plugin'
local lang = require 'share.lang'
local get_report = require 'share.report'
local check_lni_mark = require 'share.check_lni_mark'
local unpack_config = require 'backend.unpack_config'
local check_config = require 'backend.check_config'
local w2l = core()
local root = fs.current_path()
local config
local input_ar, input_proxy
local output_ar, output_proxy

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
    return err, warn
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
    input_proxy:save(type, name, buf)
    output_proxy:save(type, name, buf)
end

function w2l:file_load(type, name)
    return input_proxy:load(type, name)
end

function w2l:file_remove(type, name)
    input_proxy:remove(type, name)
    output_proxy:remove(type, name)
end

function w2l:file_pairs()
    return input_proxy:pairs()
end

function w2l:mpq_load(filename)
    return w2l.mpq_path:each_path(function(path)
        return io.load(root:parent_path() / 'data' / self.config.data_war3 / 'war3' / path / filename)
    end)
end

function w2l:defined_load(filename)
    return io.load(root:parent_path() / 'data' / self.config.data_war3 / 'war3' / 'defined' / filename)
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

local function get_io_time(map, file_count)
    local io_speed = map:get_type() == 'mpq' and 30000 or 10000
    local io_rate = math.min(0.3, file_count / io_speed)
    return io_rate
end

return function (mode)
    fs.remove(root:parent_path() / 'log' / 'report.log')
    config = unpack_config(mode)
    input = config.input

    w2l:set_messager(messager)
    if not input then
        w2l:failed(lang.script.NO_INPUT)
    end
    check_config(w2l, input)

    if input:filename():string() == '.w3x' then
        w2l:failed(lang.script.UNSUPPORTED_LNI_MARK)
    end

    w2l.messager.text(lang.script.INIT)
    w2l.messager.progress(0)

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
    if input_ar:get_type() == 'mpq' and not input_ar:get '(listfile)' then
        w2l:failed(lang.script.UNSUPPORTED_MAP)
    end

    if input_ar:get '.w3x' then
        if check_lni_mark(input_ar:get '.w3x') then
            w2l.input_mode = 'lni'
        else
            w2l:failed(lang.script.UNSUPPORTED_LNI_MARK)
        end
    end
    
    w2l:set_config(config)
    
    w2l.input_ar = input_ar
    output = config.output or default_output(config.input)
    config.output = output
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
    w2l.output_ar = output_ar
    
    input_proxy = builder.proxy(input_ar, w2l.input_mode)
    output_proxy = builder.proxy(output_ar, config.mode)

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
    w2l.progress:start(1)
    builder.save(w2l, slk.w3i, input_ar, output_ar, input_proxy, output_proxy)
    w2l.progress:finish()
    
    fs.create_directories(root:parent_path() / 'log')
    local clock = os.clock()
    messager.text(lang.script.FINISH:format(clock))
    local err, warn = exit(report)
    io.save(root:parent_path() / 'log' / 'report.log', get_report(w2l, report, config, clock, err, warn))
end
