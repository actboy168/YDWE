local get_report = require 'share.report'
local command = require 'backend.command'
local messager = require 'share.messager'
local lang = require 'share.lang'
local builder = require 'map-builder'
local core = require 'backend.sandbox_core'
local w2l = core()
local root = require 'backend.w2l_path'

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

local function absolute_path(path)
    if not path then
        return
    end
    path = fs.path(path)
    if not path:is_absolute() then
        return fs.absolute(path, base)
    end
    return fs.absolute(path)
end

local function load_file(input_ar, output_ar)
    local list = input_ar:list_file()
    local total = #list
    local count = 0
    local clock = os.clock()
    for i, name in ipairs(list) do
        local buf = input_ar:get(name)
        output_ar:set(name, buf)
        count = count + 1
        if os.clock() - clock > 0.1 then
            clock = os.clock()
            w2l.messager.text(lang.script.LOAD_MAP_FILE:format(count, total))
            w2l.progress(count / total)
        end
    end
end

return function()
    w2l:set_messager(messager)
    messager.title 'Obj'
    messager.text(lang.script.INIT)
    messager.progress(0)

    w2l.log_path = root / 'log'
    fs.remove(w2l.log_path / 'report.log')

    local input = absolute_path(command[2])
    local output = absolute_path(command[3])

    messager.text(lang.script.OPEN_MAP)
    local input_ar, err = builder.load(input)
    if not input_ar then
        w2l:failed(err)
    end
    
    local output = output or default_output(input)
    local output_ar, err = builder.load(output, 'w')
    if not output_ar then
        w2l:failed(err)
    end

    w2l.input_ar = input_ar
    w2l.output_ar = output_ar

    local wts = w2l:frontend_wts(input_ar:get 'war3map.wts')
    local w3i = w2l:frontend_w3i(input_ar:get 'war3map.w3i', wts)
    local w3f = w2l:frontend_w3f(input_ar:get 'war3campaign.w3f', wts)
    
    messager.text(lang.script.LOAD_FILE)
    w2l.progress:start(0.6)
    load_file(input_ar, output_ar)
    w2l.progress:finish()

    local plugin_loader = require 'backend.plugin'
    plugin_loader(w2l, function (source, plugin)
        w2l:add_plugin(source, plugin)
    end)

    messager.text(lang.script.CHECK_PLUGIN)
    w2l:call_plugin('on_pack', output_ar)

    messager.text(lang.script.SAVE_FILE)
    w2l.progress:start(1.0)
    builder.save(w2l, w3i, w3f, input_ar, output_ar)
    w2l.progress:finish()
    
    local clock = os.clock()
    messager.text(lang.script.FINISH:format(clock))
    local err, warn = exit(report)

    local setting = {
        input  = input,
        output = output,
        mode   = 'pack',
    }
    fs.create_directories(w2l.log_path)
    io.save(w2l.log_path / 'report.log', get_report(w2l, report, setting, clock, err, warn))
end
