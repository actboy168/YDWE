require 'filesystem'
require 'utility'
local stormlib = require 'ffi.stormlib'
local sleep = require 'ffi.sleep'
local makefile = require 'prebuilt.makefile'
local maketemplate = require 'prebuilt.maketemplate'
local prebuilt_metadata = require 'prebuilt.metadata'
local prebuilt_keydata = require 'prebuilt.keydata'
local prebuilt_search = require 'prebuilt.search'
local proto = require 'share.protocol'
local lang = require 'share.lang'
local core = require 'backend.sandbox_core'
local w3xparser = require 'w3xparser'
local messager = require 'share.messager'
local war3 = require 'share.war3'
local data_version = require 'share.data_version'
local command = require 'backend.command'
local base = require 'backend.base_path'
local root = require 'backend.w2l_path'
local w2l
local mpqs

local input
local output

local function task(f, ...)
    for i = 1, 99 do
        if pcall(f, ...) then
            return true
        end
        sleep(10)
    end
    return false
end

local result = {}

local function extract_file(path, name)
    local r = war3:extractfile(name, path / name)
    result[name] = r or false
end

local function extract_mpq(name)
    extract_file(output / 'mpq', name)
end

local function report_fail()
    local tbl = {}
    for name, res in pairs(result) do
        if res == false and not name:match '^Custom_V1' then
            table.insert(tbl, name)
        end
    end
    table.sort(tbl)
    for _, name in ipairs(tbl) do
        w2l.messager.report(lang.report.OTHER, 9, lang.script.EXPORT_FILE_FAILED .. name)
    end
end

local function extract()
    for _, dir in ipairs {'', 'Custom_V1\\'} do
        extract_mpq(dir .. 'Scripts\\Common.j')
        extract_mpq(dir .. 'Scripts\\Blizzard.j')
        extract_mpq(dir .. 'UI\\MiscData.txt')
        extract_mpq(dir .. 'Units\\MiscGame.txt')
        extract_mpq(dir .. 'Units\\MiscData.txt')
        extract_mpq(dir .. 'Units\\AbilityMetaData.slk')
        extract_mpq(dir .. 'Units\\DestructableMetaData.slk')
        extract_mpq(dir .. 'Units\\AbilitybuffMetaData.slk')
        extract_mpq(dir .. 'Units\\UpgradeMetaData.slk')
        extract_mpq(dir .. 'Units\\UnitMetaData.slk')
        extract_mpq(dir .. 'Units\\MiscMetaData.slk')
        extract_mpq(dir .. 'Doodads\\DoodadMetaData.slk')
        extract_mpq(dir .. 'UI\\UnitEditorData.txt')
        for type, slks in pairs(w2l.info.slk) do
            for _, name in ipairs(slks) do
                extract_mpq(dir .. name)
            end
        end
        for _, name in ipairs(w2l.info.txt) do
            extract_mpq(dir .. name)
        end
    end
    -- TODO: 应该放在上面的循环中？
    extract_mpq('UI\\WorldEditStrings.txt')
    extract_mpq('UI\\WorldEditGameStrings.txt')

    extract_file(output, 'UI\\TriggerData.txt')
    extract_file(output, 'UI\\TriggerStrings.txt')
end

local function create_metadata(w2l)
    local defined_meta = w2l:parse_lni(io.load(root / 'script' / 'core' / 'defined' / 'metadata.ini'))
    local meta = prebuilt_metadata(w2l, defined_meta, function (name)
        return io.load(output / 'mpq' / name)
    end)
    fs.create_directories(output / 'prebuilt')
    io.save(output / 'prebuilt' / 'metadata.ini', meta)
end

local lost_wes = {}
local reports = {}
local function get_w2l()
    w2l = core()
    w2l:set_messager(messager)

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

local function make_log(clock)
    local lines = {}
    lines[#lines+1] = lang.report.INPUT_PATH .. input:string()
    lines[#lines+1] = lang.report.OUTPUT_PATH .. output:string()
    lines[#lines+1] = lang.report.OUTPUT_MODE .. 'mpq'
    lines[#lines+1] = lang.report.TAKES_TIME:format(clock)
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
    fs.create_directories(root / 'log')
    io.save(root / 'log' / 'report.log', buf)
end

local function loader(name)
    return war3:readfile(name)
end

local function input_war3(path)
    if not path then
        path = '.'
    end
    path = fs.path(path)
    if not path:is_absolute() then
        path = fs.absolute(path, base)
    end
    return fs.absolute(path)
end

return function ()
    get_w2l()
    w2l.messager.text(lang.script.INIT)
    w2l.messager.progress(0)

    fs.remove(root / 'log' / 'report.log')
    input = input_war3(command[2])

    if not war3:open(input) then
        w2l.messager.text(lang.script.NEED_WAR3_DIR)
        return
    end
    if not war3.name then
        w2l.messager.text(lang.script.LOAD_WAR3_LANG_FAILED)
        return
    end
    output = root / 'data' / war3.name

    w2l.progress:start(0.1)
    w2l.messager.text(lang.script.CLEAN_DIR)
    if fs.exists(output) then
        if not task(fs.remove_all, output) then
            w2l.messager.text(lang.script.CREATE_DIR_FAILED:format(output:string()))
            return
        end
    end
    if not fs.exists(output) then
        if not task(fs.create_directories, output) then
            w2l.messager.text(lang.script.CREATE_DIR_FAILED:format(output:string()))
            return
        end
    end
    w2l.progress:finish()

    w2l.progress:start(0.3)
    w2l.messager.text(lang.script.EXPORT_MPQ)
    extract()
    report_fail()
    create_metadata(w2l)
    w2l.progress:finish()

    w2l.cache_metadata = w2l:parse_lni(io.load(output / 'prebuilt' / 'metadata.ini'))
    fs.create_directories(output / 'prebuilt')
    local keydata = prebuilt_keydata(w2l, loader)
    local search = prebuilt_search(w2l, loader)
    io.save(output / 'prebuilt' / 'keydata.ini', keydata)
    io.save(output / 'prebuilt' / 'search.ini', search)
    w2l.cache_metadata = nil

    io.save(output / 'version', table.concat(data_version, '\r\n'))
    local config = require 'share.config'
    config.global.data = war3.name

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

    local clock = os.clock()
    w2l.messager.text((lang.script.FINISH):format(clock))
    w2l.messager.exit('success', lang.script.MPQ_EXTRACT_DIR:format(war3.name))

    make_log(clock)
end
