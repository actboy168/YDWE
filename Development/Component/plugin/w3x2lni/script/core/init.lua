local w3xparser = require 'w3xparser'
local mpq_path = require 'mpq_path'
local lni = require 'lni'
local lml = require 'lml'
local progress = require 'progress'
local lang = require 'lang'
local proxy = require 'proxy'
local slk = w3xparser.slk
local txt = w3xparser.txt
local ini = w3xparser.ini
local pairs = pairs

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
    return lni.classics(buf, filename, ...)
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

function mt:metadata()
    if not self.cache_metadata then
        self.cache_metadata = lni.classics(load_file 'defined\\metadata.ini')
    end
    return self.cache_metadata
end

function mt:we_metadata()
    if not self.cache_we_metadata then
        if self.setting.data_meta == '${DEFAULT}' then
            self.cache_we_metadata = self.cache_metadata
        else
            self.cache_we_metadata = lni.classics(self:data_load('prebuilt\\metadata.ini'))
        end
    end
    return self.cache_we_metadata
end

function mt:keydata()
    if not keydata then
        keydata = lni.classics(self:data_load('prebuilt\\keydata.ini'))
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
        if self.setting.data_wes == '${DEFAULT}' then
            local t = ini(load_file('defined\\WorldEditStrings.txt'))['WorldEditStrings']
            for k, v in pairs(t) do
                self.editstring[k:upper()] = v
            end
        else
            local t = ini(self:data_load('mpq\\ui\\WorldEditStrings.txt'))['WorldEditStrings']
            for k, v in pairs(t) do
                self.editstring[k:upper()] = v
            end
            local t = ini(self:data_load('mpq\\ui\\WorldEditGameStrings.txt'))['WorldEditStrings']
            for k, v in pairs(t) do
                self.editstring[k:upper()] = v
            end
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
        local str = w2l:data_load(('prebuilt\\%s\\%s.ini'):format(w2l.setting.version, name))
        if str then
            default[name] = lni.classics(str)
        else
            need_build = true
            break
        end
    end
    if need_build then
        default = w2l:frontend_buildslk()
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
        self.info = require 'info'
        return self.info
    end
    if hasFile('slk.'..name) then
        self[name] = require('slk.'..name)
        return self[name]
    end
    self.loaded[name] = true
    return nil
end

function mt:mpq_load(filename)
    return self.mpq_path:each_path(function(path)
        return self:data_load(('mpq\\%s\\%s'):format(path, filename))
    end)
end

function mt:add_plugin(source, plugin)
    self.plugins[#self.plugins + 1] = plugin
    self.messager.report(lang.report.OTHER, 9, lang.report.USED_PLUGIN:format(plugin.info.name, source), plugin.info.description)
end

function mt:call_plugin(event, ...)
    for _, plugin in ipairs(self.plugins) do
        if plugin[event] then
            local ok, res = pcall(plugin[event], plugin, self, ...)
            if ok then
                if res ~= nil then
                    return res
                end
            else
                self.messager.report(lang.report.OTHER, 2, lang.report.PLUGIN_FAILED:format(plugin.info.name), res)
            end
        end
    end
end

function mt:init_proxy()
    if self.inited_proxy then
        return
    end
    self.inited_proxy = true
    self.input_proxy = proxy(self, self.input_ar, self.input_mode, 'input')
    self.output_proxy = proxy(self, self.output_ar, self.setting.mode, 'output')
    
    if self:file_load('w3x2lni', 'locale\\w3i.lng') then
        lang:set_lng_file('w3i', self:file_load('w3x2lni', 'locale\\w3i.lng'))
    end
    if self:file_load('w3x2lni', 'locale\\lml.lng') then
        lang:set_lng_file('lml', self:file_load('w3x2lni', 'locale\\lml.lng'))
    end
    
    if self.setting.mode == 'lni' then
        self:file_save('w3x2lni', 'locale\\w3i.lng', lang:get_lng_file 'w3i')
        self:file_save('w3x2lni', 'locale\\lml.lng', lang:get_lng_file 'lml')
    else
        self:file_remove('w3x2lni', 'locale\\w3i.lng')
        self:file_remove('w3x2lni', 'locale\\lml.lng')
    end
end

function mt:file_save(type, name, buf)
    self:init_proxy()
    self.input_proxy:save(type, name, buf)
    self.output_proxy:save(type, name, buf)
end

function mt:file_load(type, name)
    self:init_proxy()
    return self.input_proxy:load(type, name)
end

function mt:file_remove(type, name)
    self:init_proxy()
    self.input_proxy:remove(type, name)
    self.output_proxy:remove(type, name)
end

function mt:file_pairs()
    self:init_proxy()
    return self.input_proxy:pairs()
end

function mt:save()
    local save = require 'map-builder.save'
    save(self)
    local total = self.input_ar:number_of_files()
    local count = 0
    for _ in pairs(self.input_ar) do
        count = count + 1
    end
    if count ~= total then
        self.messager.report(lang.report.ERROR, 1, lang.report.FILE_LOST:format(total - count), lang.report.FILE_LOST_HINT)
        self.messager.report(lang.report.ERROR, 1, lang.report.FILE_READ:format(count, total))
    end
end

function mt:failed(msg)
    self.messager.exit('failed', msg or lang.script.UNKNOWN_REASON)
    os.exit(1, true)
end

mt.setting = {}

local function toboolean(v)
    if v == 'true' or v == true then
        return true
    elseif v == 'false' or v == false then
        return false
    end
    return nil
end

function mt:set_setting(setting)
    local default = self:parse_lni(load_file 'config.ini')
    local setting = setting or {}
    local dir

    local function choose(k, f)
        local a = setting[k]
        local b = dir and dir[k]
        if f then
            a = f(a)
            b = f(b)
        end
        if a == nil then
            setting[k] = b
        else
            setting[k] = a
        end
    end
    dir = default.global
    choose('data')
    choose('data_meta')
    choose('data_wes')
    dir = default[setting.mode]
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
    
    self.setting = setting
    
    self.mpq_path = mpq_path()
    if self.setting.version == 'Custom' then
        self.mpq_path:open 'Custom_V1'
    end
    self.data_load = require 'data_load'
end

function mt:set_messager(messager)
    if type(messager) == 'table' then
        if getmetatable(messager) then
            self.messager = messager
        else
            self.messager = setmetatable(messager, { __index = function () return function () end end })
        end
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
    self.plugins = {}
    self.lang = require 'lang'
    self:set_messager(function () end)
    self:set_setting()
    return self
end
