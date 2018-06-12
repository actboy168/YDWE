local get_report = require 'share.report'
local unpack_setting = require 'backend.unpack_setting'
local messager = require 'share.messager'
local lang = require 'share.lang'
local builder = require 'map-builder'
local core = require 'backend.sandbox_core'
local w2l = core()
local root = fs.current_path()

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
    return input:parent_path() / (input:filename():string() .. '.w3x')
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

local function get_io_time(map, file_count)
    local io_speed = map:get_type() == 'mpq' and 30000 or 10000
    local io_rate = math.min(0.3, file_count / io_speed)
    return io_rate
end

return function()
    w2l.log_path = root:parent_path() / 'log'
    w2l:set_messager(messager)
    w2l.messager.text(lang.script.INIT)
    w2l.messager.progress(0)

    fs.remove(w2l.log_path / 'report.log')

    local setting = unpack_setting(w2l, mode)

    setting.mode = 'pack'
    messager.title 'Obj'
    messager.text(lang.script.OPEN_MAP)
    local err
    local input_ar, err = builder.load(setting.input)
    if not input_ar then
        w2l:failed(err)
    end
    if input_ar:get_type() == 'mpq' and not input_ar:get '(listfile)' then
        w2l:failed(lang.script.UNSUPPORTED_MAP)
    end
    
    w2l:set_setting(setting)
    
    w2l.input_ar = input_ar
    local output = setting.output or default_output(setting.input)
    setting.output = output

    output_ar, err = builder.load(output, 'w')
    if not output_ar then
        w2l:failed(err)
    end
    w2l.output_ar = output_ar

    messager.text(lang.script.CHECK_PLUGIN)
    w2l:call_plugin 'on_convert'

    local slk = {}
    local file_count = input_ar:number_of_files()
    local input_rate = get_io_time(input_ar, file_count)
    local output_rate = get_io_time(output_ar, file_count)

    slk.w3i = w2l:frontend_w3i(input_ar:get 'war3map.w3i')
    
    messager.text(lang.script.LOAD_FILE)
    w2l.progress:start(input_rate)
    w2l:save()
    w2l.progress:finish()

    messager.text(lang.script.SAVE_FILE)
    w2l.progress:start(1)
    builder.save(w2l, slk.w3i, input_ar, output_ar)
    w2l.progress:finish()
    
    local clock = os.clock()
    messager.text(lang.script.FINISH:format(clock))
    local err, warn = exit(report)
    fs.create_directories(w2l.log_path)
    io.save(w2l.log_path / 'report.log', get_report(w2l, report, setting, clock, err, warn))
end
