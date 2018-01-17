local w3xparser = require 'w3xparser'
local mpq_path = require 'mpq_path'
local lni = require 'lni-c'
local progress = require 'progress'
local slk = w3xparser.slk
local txt = w3xparser.txt
local ini = w3xparser.ini
local pairs = pairs
local string_lower = string.lower

local mt = {}

local metadata
local keydata
local miscnames

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
    return lni(load_file('defined\\' .. name .. '.ini'))
end

function mt:metadata()
    if not metadata then
        metadata = self:defined 'metadata'
    end
    return metadata
end

function mt:keydata()
    if not keydata then
        keydata = self:defined 'keydata'
    end
    return keydata
end

function mt:miscnames()
    if not miscnames then
        miscnames = self:defined 'miscnames'
    end
    return miscnames
end

function mt:get_editstring(str)
    -- TODO: WESTRING不区分大小写，不过我们把WorldEditStrings.txt改了，暂时不会出现问题
    if not self.editstring then
        self.editstring = ini(self:mpq_load('UI\\WorldEditStrings.txt'))['WorldEditStrings']
    end
    if not self.editstring[str] then
        return str
    end
    repeat
        str = self.editstring[str]
    until not self.editstring[str]
    return str:gsub('%c+', '')
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
    return content:gsub('TRIGSTR_(%d+)', function(i)
        local str_data = wts[tonumber(i)]
        if not str_data then
            self.message('-report|9其他', '没有找到字符串定义:', ('TRIGSTR_%03d'):format(i))
            return
        end
        local text = str_data.text
        if fmter then
            text = fmter(text)
        end
        if max and #text > max then
            return self:save_wts(wts, text, reason)
        end
        return text
    end)
end

function mt:save_wts(wts, text, reason)
    self.message('-report|7保存到wts中的文本', reason)
    self.message('-tip', '文本保存在wts中会导致加载速度变慢: ', (text:sub(1, 1000):gsub('\r\n', ' ')))
    if text:find('}', 1, false) then
        self.message('-report|2警告', '文本中的"}"被修改为了"|"')
        self.message('-tip', (text:sub(1, 1000):gsub('\r\n', ' ')))
        text = text:gsub('}', '|')
    end
    local index = #wts.mark + 1
    wts.mark[index] = text
    return ('TRIGSTR_%03d'):format(index-1)
end

function mt:refresh_wts(wts)
    local lines = {}
    for index, text in ipairs(wts.mark) do
        lines[#lines+1] = ('STRING %d\r\n{\r\n%s\r\n}'):format(index-1, text)
    end
    return table.concat(lines, '\r\n\r\n')
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
    local suc, res = pcall(require, 'slk.'..name)
    if suc then
        self[name] = res
        return res
    end
    local suc, res = pcall(require, 'other.'..name)
    if suc then
        self[name] = res
        return res
    end
    self.loaded[name] = true
    return nil
end

local function new_path()
    local mt = {}
    local paths = {'\\'}
    local mpqs = {}
    local function update()
        paths = {'\\'}
        for i = #mpqs, 1, -1 do
            local path = mpqs[i]
            local max = #paths
            table.insert(paths, '\\' .. path .. '\\')
            for i = 2, max do
                table.insert(paths, '\\' .. path .. paths[i])
            end
        end
    end
    function mt:open(path)
        table.insert(mpqs, path)
        update()
    end
    function mt:close(path)
        for i, mpq in ipairs(mpqs) do
            if mpq == path then
                table.remove(mpqs, i)
                update()
                return
            end
        end
    end
    function mt:each_path(callback)
        for i = #paths, 1, -1 do
            local res = callback(paths[i])
            if res then
                return res
            end
        end
    end
    return mt
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

mt.config = {}
function mt:set_config(config)
    self.config = config
    self.mpq_path = mpq_path()
    self.mpq_path:open(config.mpq)
    self.mpq_path:open(config.lang)
    if self.config.version == 'Melee' then
        self.mpq_path:open 'Melee_V1'
    else
        self.mpq_path:open 'Custom_V1'
    end
end

function mt:set_messager(messager)
    self.message = messager
    self.progress:set_messager(messager)
end

return function ()
    local self = setmetatable({}, mt)
    self.progress = progress()
    self.loaded = {}
    self:set_messager(function () end)
    return self
end
