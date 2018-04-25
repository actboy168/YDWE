require 'filesystem'
require 'utility'
local stormlib = require 'ffi.stormlib'
local sleep = require 'ffi.sleep'
local makefile = require 'prebuilt.makefile'
local maketemplate = require 'prebuilt.maketemplate'
local prebuilt_metadata = require 'prebuilt.metadata'
local proto = require 'tool.protocol'
local lang = require 'tool.lang'
local file_version = require 'ffi.file_version'
local core = require 'backend.sandbox_core'
local unpack_config = require 'backend.unpack_config'
local w3xparser = require 'w3xparser'
local w2l
local mpq_name
local root = fs.current_path()

local language_map = {
    [0x00000409] = 'enUS',
    [0x00000809] = 'enGB',
    [0x0000040c] = 'frFR',
    [0x00000407] = 'deDE',
    [0x0000040a] = 'esES',
    [0x00000410] = 'itIT',
    [0x00000405] = 'csCZ',
    [0x00000419] = 'ruRU',
    [0x00000415] = 'plPL',
    [0x00000416] = 'ptBR',
    [0x00000816] = 'ptPT',
    [0x0000041f] = 'tkTK',
    [0x00000411] = 'jaJA',
    [0x00000412] = 'koKR',
    [0x00000404] = 'zhTW',
    [0x00000804] = 'zhCN',
    [0x0000041e] = 'thTH',
}

local function mpq_language(config)
    if not config then
        return nil
    end
    local id = config:match 'LANGID=(0x[%x]+)'
    if not id then
        return nil
    end
    return language_map[tonumber(id)]
end

local function war3_ver(input)
    local exe_path = input / 'War3.exe'
    if fs.exists(exe_path) then
        local ver = file_version(exe_path:string())
        if ver.major > 1 or ver.minor >= 29 then
            return ('%d.%d.%d'):format(ver.major, ver.minor, ver.revision)
        end
    end
    local exe_path = input / 'Warcraft III.exe'
    if fs.exists(exe_path) then
        local ver = file_version(exe_path:string())
        if ver.major > 1 or ver.minor >= 29 then
            return ('%d.%d.%d'):format(ver.major, ver.minor, ver.revision)
        end
    end
    local dll_path = input / 'Game.dll'
    if fs.exists(dll_path) then
        local ver = file_version(dll_path:string())
        return ('%d.%d.%d'):format(ver.major, ver.minor, ver.revision)
    end
    return nil
end

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

local function extract_mpq(mpqs)
    local info = w2l:parse_lni(assert(io.load(fs.current_path() / 'core' / 'info.ini')))
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

local function create_metadata(w2l, mpqs)
    local meta = prebuilt_metadata(w2l, function (filename)
        if filename == 'doodadmetadata.slk' then
            return mpqs:load_file('Doodads\\' .. filename)
        else
            return mpqs:load_file('Units\\' .. filename)
        end
    end)
    local meta_path = root:parent_path() / 'data' / mpq_name / 'we'
    fs.create_directories(meta_path)
    io.save(meta_path / 'metadata.ini', meta)
end

local function create_wes(w2l, mpqs)
    local wes = mpqs:load_file 'ui\\WorldEditStrings.txt'
    local wes_path = root:parent_path() / 'data' / mpq_name / 'we'
    fs.create_directories(wes_path)
    io.save(wes_path / 'WorldEditStrings.txt', wes)
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

return function ()
    local config = unpack_config()
    local input = config.input
    get_w2l()

    if not fs.is_directory(input) then
        w2l.messager.text(lang.script.NEED_WAR3_DIR)
        return
    end
    local ver = war3_ver(input)
    if not ver then
        w2l.messager.text(lang.script.NEED_WAR3_DIR)
        return
    end
    local mpqs = open_mpq(input)
    if not mpqs then
        return
    end
    local lg = mpq_language(mpqs:load_file 'config.txt')
    if lg then
        mpq_name = lg .. '-' .. ver
    else
        mpq_name = input:filename():string()
    end

    w2l.progress:start(0.1)
    w2l.messager.text(lang.script.CLEAN_DIR)
    local mpq_path = fs.current_path():parent_path() / 'data' / mpq_name / 'war3'
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
    extract_mpq(mpqs)
    report_fail()
    w2l.progress:finish()

    w2l.progress:start(0.3)
    create_metadata(w2l, mpqs)
    create_wes(w2l, mpqs)
    w2l.progress:finish()

    w2l.progress:start(0.4)
    makefile(w2l, mpq_name, 'Melee')
    w2l.progress:finish()
    w2l.progress:start(0.65)
    maketemplate(w2l, mpq_name, 'Melee')
    w2l.progress:finish()
    w2l.progress:start(0.75)
    makefile(w2l, mpq_name, 'Custom')
    w2l.progress:finish()
    w2l.progress:start(1.0)
    maketemplate(w2l, mpq_name, 'Custom')
    w2l.progress:finish()

    local config = require 'tool.config' ()
    config.global.data_war3 = mpq_name
    if config.global.data_ui ~= '${YDWE}' then
        config.global.data_ui = mpq_name
    end
    if config.global.data_meta ~= '${DEFAULT}' then
        config.global.data_meta = mpq_name
    end
    if config.global.data_wes ~= '${DEFAULT}' then
        config.global.data_wes = mpq_name
    end
    w2l.messager.text((lang.script.FINISH):format(os.clock()))
    w2l.messager.exit('success', lang.script.MPQ_EXTRACT_DIR:format(mpq_name))

    make_log()
end
