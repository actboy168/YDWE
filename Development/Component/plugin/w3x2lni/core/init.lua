local w3xparser = require 'w3xparser'
local mpq_path = require 'mpq_path'
local lni = require 'lni'
local lml = require 'lml'
local progress = require 'progress'
local lang = require 'lang'
local slk = w3xparser.slk
local txt = w3xparser.txt
local ini = w3xparser.ini
local pairs = pairs
local string_lower = string.lower

local mt = {}

local keydata

local function load_file(path)
    local f = io.open(path)
    if f then
        local buf = f:read 'a'
        f:close()
        return buf
    end
    return nil
end

function mt:parse_lni(buf, filename, ...)
    return lni(buf, filename, ...)
end

function mt:parse_lml(buf)
    return lml(buf)
end

function mt:parse_slk(buf)
    return slk(buf)
end

function mt:parse_txt(...)
    return txt(...)
end

function mt:parse_ini(buf)
    return ini(buf)
end


function mt:defined(name)
    return lni(self:mpq_load('defined\\' .. name .. '.ini'))
end

function mt:metadata()
    if not self.cache_metadata then
        if self.config.mode ~= 'obj' or self.config.data_meta == '${DEFAULT}' then
            self.cache_metadata = lni(load_file 'defined\\metadata.ini')
        else
            self.cache_metadata = lni(self:meta_load 'metadata.ini')
        end
    end
    return self.cache_metadata
end

function mt:keydata()
    if not keydata then
        keydata = self:defined 'keydata'
    end
    return keydata
end

function mt:get_editstring(source)
    local str = source:upper()
    if str:sub(1, 9) ~= 'WESTRING_' then
        return source
    end
    if not self.editstring then
        self.editstring = {}
        local t
        if self.config.data_wes == '${YDWE}' then
            t = ini(load_file('defined\\WorldEditStrings.txt'))['WorldEditStrings']
        else
            t = ini(self:wes_load('WorldEditStrings.txt'))['WorldEditStrings']
        end
        for k, v in pairs(t) do
            self.editstring[k:upper()] = v
        end
        if self.config.data_wes ~= '${YDWE}' then
            t = ini(self:wes_load('WorldEditGameStrings.txt'))['WorldEditStrings']
        end
        for k, v in pairs(t) do
            self.editstring[k:upper()] = v
        end
    end
    if self.editstring[str] then
        repeat
            str = self.editstring[str]
        until not self.editstring[str]
        return str:gsub('%c+', '')
    end
    local text = lang.wes[str]
    if text ~= str then
        return text
    end
    if not self.editstring_reported then
        self.editstring_reported = {}
    end
    if not self.editstring_reported[str] then
        self.editstring_reported[str] = true
        --self.editstring_reported[#self.editstring_reported+1] = str
        self.messager.report(lang.report.OTHER, 9, lang.report.NO_WES_STRING, source)
    end
    return source
end

local function create_default(w2l)
    local default = {}
    local need_build = false
    for _, name in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'txt', 'misc'} do
        local str = w2l:prebuilt_load(name .. '.ini')
        if str then
            default[name] = lni(str)
        else
            need_build = true
            break
        end
    end
    if need_build then
        default = w2l:build_slk()
        w2l.messager.report(lang.report.OTHER, 9, lang.report.NO_PREBUILT)
    end
    return default
end

function mt:get_default(create)
    if create then
        return create_default(self)
    end
    if not self.default_data then
        self.default_data = create_default(self)
    end
    return self.default_data
end

-- 同时有英文逗号和英文双引号的字符串存在txt里会解析出错
-- 包含右大括号的字符串存在wts里会解析出错
-- 超过256字节的字符串存在二进制里会崩溃

function mt:load_wts(wts, content, max, reason, fmter)
    if not wts then
        return content
    end
    local str, count = content:gsub('TRIGSTR_(%d+)', function(i)
        local str_data = wts[tonumber(i)]
        if not str_data then
            self.messager.report(lang.report.OTHER, 9, lang.report.NO_TRIGSTR:format(i))
            return
        end
        local text = str_data.text
        if fmter then
            text = fmter(text)
        end
        if max and #text > max then
            return self:save_wts(wts, str_data.text, reason)
        end
        return text
    end)
    if count == 0 and max and #str > max then
        str = self:save_wts(wts, str, reason)
    end
    return str
end

function mt:save_wts(wts, text, reason)
    self.messager.report(lang.report.TEXT_IN_WTS, 7, reason, ('%s\r\n%s...\r\n-------------------------'):format(lang.report.TEXT_IN_WTS_HINT, text:sub(1, 1000)))
    if text:find('}', 1, false) then
        self.messager.report(lang.report.WARN, 2, lang.report.WTS_NEED_ESCAPE, text:sub(1, 1000))
        text = text:gsub('}', '|')
    end
    local index = #wts.mark + 1
    wts.mark[index] = text
    return ('TRIGSTR_%03d'):format(index-1)
end

function mt:refresh_wts(wts)
    if not wts then
        return
    end
    local lines = {}
    for index, text in ipairs(wts.mark) do
        lines[#lines+1] = ('STRING %d\r\n{\r\n%s\r\n}'):format(index-1, text)
    end
    return table.concat(lines, '\r\n\r\n')
end

local function hasFile(name)
    return nil ~= package.searchpath(name, package.path)
end

function mt:__index(name)
    local value = mt[name]
    if value then
        return value
    end
    if self.loaded[name] then
        return nil
    end
    if name == 'info' then
        self.info = lni(assert(load_file('info.ini')), 'info')
        return self.info
    end
    if hasFile('slk.'..name) then
        self[name] = require('slk.'..name)
        return self[name]
    end
    if hasFile('other.'..name) then
        self[name] = require('other.'..name)
        return self[name]
    end
    self.loaded[name] = true
    return nil
end

function mt:map_load(filename)
    return nil
end

function mt:map_save(filename, buf)
end

function mt:map_remove(filename)
end

function mt:mpq_load(filename)
    return nil
end

function mt:prebuilt_load(filename)
    return nil
end

function mt:trigger_data()
    return nil
end

function mt:call_plugin()
end

function mt:file_save(type, name, buf)
    if type == 'table' then
        self:map_save(self.info.lni_dir[name][1], buf)
    elseif type == 'trigger' then
        self:map_save('war3map.wtg.lml/' .. name, buf)
    elseif type == 'map' then
        self:map_save(name, buf)
    elseif type == 'scirpt' then
        if self.config.export_lua then
            self:map_save(name, buf)
        end
    end
end

function mt:file_load(type, name)
    if type == 'table' then
        return self:map_load(self.info.lni_dir[name][1])
    elseif type == 'trigger' then
        return self:map_load('war3map.wtg.lml/' .. name)
    elseif type == 'map' then
        return self:map_load(name)
    end
end

function mt:file_remove(type, name)
    if type == 'table' then
        self:map_remove(self.info.lni_dir[name][1], buf)
    elseif type == 'trigger' then
        self:map_remove('war3map.wtg.lml/' .. name, buf)
    elseif type == 'map' then
        self:map_remove(name, buf)
    end
end

function mt:failed(msg)
    self.messager.exit('failed', msg or lang.script.UNKNOWN_REASON)
    os.exit(1, true)
end

mt.config = {}

local function toboolean(v)
    if v == 'true' or v == true then
        return true
    elseif v == 'false' or v == false then
        return false
    end
    return nil
end

function mt:set_config(config)
    local default = self:parse_lni(load_file 'config.ini')
    local config = config or {}
    local dir

    local function choose(k, f)
        local a = config[k]
        local b = dir and dir[k]
        if f then
            a = f(a)
            b = f(b)
        end
        if a == nil then
            config[k] = b
        else
            config[k] = a
        end
    end
    dir = default.global
    choose('data_war3')
    choose('data_meta')
    choose('data_wes')
    dir = default[config.mode]
    choose('read_slk', toboolean)
    choose('find_id_times', math.tointeger)
    choose('remove_same', toboolean)
    choose('remove_we_only', toboolean)
    choose('remove_unuse_object', toboolean)
    choose('mdx_squf', toboolean)
    choose('slk_doodad', toboolean)
    choose('optimize_jass', toboolean)
    choose('confused', toboolean)
    choose('confusion')
    choose('target_storage')
    choose('computed_text', toboolean)
    choose('export_lua', toboolean)
    
    self.config = config
    
    self.mpq_path = mpq_path()
    if self.config.version == 'Melee' then
        self.mpq_path:open 'Melee_V1'
    else
        self.mpq_path:open 'Custom_V1'
    end
end

function mt:set_messager(messager)
    if type(messager) == 'table' then
        self.messager = setmetatable(messager, { __index = function () return function () end end })
    else
        self.messager = setmetatable({}, { __index = function (_, tp)
            return function (...)
                messager(tp, ...)
            end
        end })
    end
    self.progress:set_messager(self.messager)
end

return function ()
    local self = setmetatable({}, mt)
    self.progress = progress()
    self.loaded = {}
    self:set_messager(function () end)
    self:set_config()
    return self
end
