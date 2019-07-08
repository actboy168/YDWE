local get_report = require 'share.report'
local command = require 'backend.command'
local messager = require 'share.messager'
local lang = require 'share.lang'
local builder = require 'map-builder'
local core = require 'backend.sandbox_core'
local w2l = core()
local root = require 'backend.w2l_path'
local base = require 'backend.base_path'

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
    return input:parent_path() / (input:stem():string())
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

local function loadListFile(buf)
    if not buf then
        return nil
    end
    local list = {}
    local start = 1
    while true do
        local pos = buf:find('\r\n', start)
        if not pos then
            list[#list+1] = buf:sub(start)
            break
        end
        list[#list+1] = buf:sub(start, pos-1)
        start = pos + 2
    end
    return list
end

local function mergeLists(...)
    local result = {}
    local mark = {}
    local lists = table.pack(...)
    for i = 1, lists.n do
        local list = lists[i]
        if list then
            for _, name in ipairs(list) do
                local lname = name:lower()
                if not mark[lname] then
                    mark[lname] = true
                    result[#result+1] = name
                end
            end
        end
    end
    return result
end

local function loadStaticfile()
    local list = {}
    local function search_tbl(tbl)
        for _, v in pairs(tbl) do
            if type(v) == 'table' then
                search_tbl(v)
            elseif type(v) == 'string' then
                list[#list+1] = v
            end
        end
    end
    search_tbl(w2l.info)
    return list
end

local function load_file(input_ar, output_ar)
    local extraPath = absolute_path(command['listfile'])
    local extraList
    if extraPath then
        extraList = loadListFile(io.load(extraPath))
    end

    local mapList = loadListFile(input_ar:get('(listfile)'))
    local staticList = loadStaticfile()

    local list = mergeLists(mapList, extraList, staticList)

    local total = #list
    local clock = os.clock()
    for i, name in ipairs(list) do
        local buf = input_ar:get(name)
        if buf then
            output_ar:set(name, buf)
        end
        if os.clock() - clock > 0.1 then
            clock = os.clock()
            w2l.messager.text(lang.script.LOAD_MAP_FILE:format(i, total))
            w2l.progress(i / total)
        end
    end

    local count = 0
    for _ in pairs(output_ar) do
        count = count + 1
    end

    if count ~= input_ar:number_of_files() then
        return false, lang.script.NEED_LIST_FILE
    end

    return true
end

local function remove(path)
    if fs.is_directory(path) then
        for c in path:list_directory() do
            remove(c)
        end
    end
    fs.remove(path)
end

local function createDir(path)
    if fs.exists(path) then
        remove(path)
    end
    fs.create_directories(path)
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

    if not input then
        w2l:failed(lang.script.OPEN_FAILED_NO_EXISTS)
    end

    messager.text(lang.script.OPEN_MAP)
    local input_ar, err = builder.load(input)
    if not input_ar then
        w2l:failed(err)
    end

    local output = output or default_output(input)
    createDir(output)
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
    local suc, err = load_file(input_ar, output_ar)
    if not suc then
        w2l:failed(err)
    end
    w2l.progress:finish()

    local plugin_loader = require 'backend.plugin'
    plugin_loader(w2l, function (source, plugin)
        w2l:add_plugin(source, plugin)
    end)

    messager.text(lang.script.CHECK_PLUGIN)
    w2l:call_plugin('on_unpack', output_ar)

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
