require 'filesystem'
require 'utility'
local stormlib = require 'ffi.stormlib'
local sleep = require 'ffi.sleep'
local makefile = require 'prebuilt.makefile'
local maketemplate = require 'prebuilt.maketemplate'
local prebuilt_metadata = require 'prebuilt.metadata'
local prebuilt_keydata = require 'prebuilt.keydata'
local prebuilt_search = require 'prebuilt.search'
local proto = require 'tool.protocol'
local lang = require 'tool.lang'
local core = require 'backend.sandbox_core'
local unpack_config = require 'backend.unpack_config'
local w3xparser = require 'w3xparser'
local messager = require 'tool.messager'
local war3_name = require 'tool.war3_name'
local w2l
local mpq_name
local mpqs
local root = fs.current_path()

local function task(f, ...)
    for i = 1, 99 do
        if pcall(f, ...) then
            return true
        end
        sleep(10)
    end
    return false
end

local mpq_names = {
    'War3.mpq',
    'War3x.mpq',
    'War3xLocal.mpq',
    'War3Patch.mpq',
}

local result = {} 
local function open_mpq(dir)
    local mpqs = {}
    for i, name in ipairs(mpq_names) do
        mpqs[#mpqs+1] = stormlib.open(dir / name, true)
    end

    local obj = {}

    function obj:extract_file(type, name)
        local path = root:parent_path() / 'data' / mpq_name / type / name
        for i = #mpqs, 1, -1 do
            local mpq = mpqs[i]
            if mpq:has_file(name) then
                result[name] = mpq:extract(name, path)
                return
            end
        end
        if not name:match '^Custom_V1' then
            result[name] = false
        end
    end

    function obj:load_file(name)
        for i = #mpqs, 1, -1 do
            local mpq = mpqs[i]
            if mpq:has_file(name) then
                return mpq:load_file(name)
            end
        end
        return nil
    end

    return obj
end

local function report_fail()
    local tbl = {}
    for name, res in pairs(result) do
        if res == false then
            table.insert(tbl, name)
        end
    end
    table.sort(tbl)
    for _, name in ipairs(tbl) do
        w2l.messager.report(lang.report.OTHER, 9, lang.script.EXPORT_FILE_FAILED .. name)
    end
end

local function extract_mpq()
    local info = w2l:parse_lni(assert(io.load(root / 'core' / 'info.ini')))
    for _, root in ipairs {'', 'Custom_V1\\'} do
        mpqs:extract_file('war3', root .. 'Scripts\\Common.j')
        mpqs:extract_file('war3', root .. 'Scripts\\Blizzard.j')
        
        mpqs:extract_file('war3', root .. 'UI\\MiscData.txt')

        mpqs:extract_file('war3', root .. 'Units\\MiscGame.txt')
        mpqs:extract_file('war3', root .. 'Units\\MiscData.txt')

        mpqs:extract_file('we', root .. 'ui\\TriggerData.txt')
        mpqs:extract_file('we', root .. 'ui\\TriggerStrings.txt')

        for type, slks in pairs(info.slk) do
            for _, name in ipairs(slks) do
                mpqs:extract_file('war3', root .. name)
            end
        end

        for _, name in ipairs(info.txt) do
            mpqs:extract_file('war3', root .. name)
        end
    end
end

local function create_metadata(w2l, loader)
    local defined_meta = w2l:parse_lni(io.load(root / 'core' / 'defined' / 'metadata.ini'))
    local meta = prebuilt_metadata(w2l, defined_meta, loader)
    local meta_path = root:parent_path() / 'data' / mpq_name / 'we'
    fs.create_directories(meta_path)
    io.save(meta_path / 'metadata.ini', meta)
end

local function create_wes(w2l)
    local wes_path = root:parent_path() / 'data' / mpq_name / 'we'
    fs.create_directories(wes_path)
    local wes = mpqs:load_file 'ui\\WorldEditStrings.txt'
    local wegs = mpqs:load_file 'ui\\WorldEditGameStrings.txt'
    io.save(wes_path / 'WorldEditStrings.txt', wes)
    io.save(wes_path / 'WorldEditGameStrings.txt', wegs)
end

local lost_wes = {}
local reports = {}
local function get_w2l()
    w2l = core()
    w2l:set_messager(messager)

    function w2l:mpq_load(filename)
        local mpq_path = root:parent_path() / 'data' / mpq_name / 'war3'
        return self.mpq_path:each_path(function(path)
            return io.load(mpq_path / path / filename)
        end)
    end

    function w2l:defined_load(filename)
        local mpq_path = root:parent_path() / 'data' / mpq_name / 'war3'
        return io.load(mpq_path / 'defined' / filename)
    end

    function w2l:wes_load(filename)
        return io.load(root:parent_path() / 'data' / mpq_name / 'we' / filename)
    end

    function messager.report(_, _, str, tip)
        if str == lang.report.NO_WES_STRING then
            lost_wes[tip] = true
        else
            reports[#reports+1] = str
        end
    end
end

local function sortpairs(t)
    local keys = {}
    for k in pairs(t) do
        keys[#keys+1] = k
    end
    table.sort(keys)
    local i = 0
    return function ()
        i = i + 1
        local k = keys[i]
        return k, t[k]
    end
end

local function make_log()
    local lines = {}
    if #reports > 0 then
        for _, rep in ipairs(reports) do
            lines[#lines+1] = rep
        end
        lines[#lines+1] = ''
    end
    if next(lost_wes) then
        lines[#lines+1] = lang.script.UNEXIST_WES_IN_MPQ
        for v in sortpairs(lost_wes) do
            lines[#lines+1] = v
        end
        lines[#lines+1] = ''
    end
    local buf = table.concat(lines, '\r\n')
    io.save(root:parent_path() / 'log' / 'mpq.log', buf)
end

local function loader(name)
    return mpqs:load_file(name)
end

return function ()
    local config = unpack_config()
    local input = config.input
    get_w2l()

    mpq_name = war3_name(input)
    if not mpq_name then
        w2l.messager.text(lang.script.NEED_WAR3_DIR)
        return
    end
    mpqs = open_mpq(input)
    if not mpqs then
        return
    end
    fs.create_directories(root:parent_path() / 'data' / mpq_name)
    local config = require 'tool.config' ()
    config.global.data_war3 = mpq_name
    if config.global.data_ui ~= '${YDWE}' then
        config.global.data_ui = mpq_name
    end
    if config.global.data_meta ~= '${DEFAULT}' then
        config.global.data_meta = mpq_name
    end
    if config.global.data_wes ~= '${YDWE}' then
        config.global.data_wes = mpq_name
    end

    w2l.progress:start(0.1)
    w2l.messager.text(lang.script.CLEAN_DIR)
    local mpq_path = root:parent_path() / 'data' / mpq_name / 'war3'
    if fs.exists(mpq_path) then
        if not task(fs.remove_all, mpq_path) then
            w2l.messager.text(lang.script.CREATE_DIR_FAILED:format(mpq_path:string()))
            return
        end
    end
    if not fs.exists(mpq_path) then
        if not task(fs.create_directories, mpq_path) then
            w2l.messager.text(lang.script.CREATE_DIR_FAILED:format(mpq_path:string()))
            return
        end
    end
    w2l.progress:finish()

    w2l.progress:start(0.2)
    w2l.messager.text(lang.script.EXPORT_MPQ)
    extract_mpq()
    report_fail()
    w2l.progress:finish()

    w2l.progress:start(0.3)
    create_metadata(w2l, loader)
    create_wes(w2l)
    w2l.progress:finish()

    w2l.cache_metadata = w2l:parse_lni(io.load(root:parent_path() / 'data' / mpq_name / 'we' / 'metadata.ini'))
    local dir = root:parent_path() / 'data' / mpq_name / 'war3' / 'defined'
    fs.create_directories(dir)
    local keydata = prebuilt_keydata(w2l, loader)
    local search = prebuilt_search(w2l, loader)
    io.save(dir / 'keydata.ini', keydata)
    io.save(dir / 'search.ini', search)
    w2l.cache_metadata = nil

    w2l.progress:start(0.4)
    local slk = makefile(w2l, 'Melee')
    w2l.progress:finish()
    w2l.progress:start(0.65)
    maketemplate(w2l, 'Melee', slk)
    w2l.progress:finish()
    w2l.progress:start(0.75)
    local slk = makefile(w2l, 'Custom')
    w2l.progress:finish()
    w2l.progress:start(1.0)
    maketemplate(w2l, 'Custom', slk)
    w2l.progress:finish()

    w2l.messager.text((lang.script.FINISH):format(os.clock()))
    w2l.messager.exit('success', lang.script.MPQ_EXTRACT_DIR:format(mpq_name))

    make_log()
end
