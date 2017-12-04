local w3xparser = require 'w3xparser'
local loader = require 'loader'
local lni = require 'lni-c'
local slk = w3xparser.slk
local txt = w3xparser.txt
local ini = w3xparser.ini
local pairs = pairs
local string_lower = string.lower

local mt = {}

local metadata
local keydata
local editstring
local default
local miscnames
local wts

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

function mt:editstring(str)
    -- TODO: WESTRING不区分大小写，不过我们把WorldEditStrings.txt改了，暂时不会出现问题
    if not editstring then
        editstring = ini(loader:mpq_load(self.mpq .. '\\UI\\WorldEditStrings.txt'))['WorldEditStrings']
    end
    if not editstring[str] then
        return str
    end
    repeat
        str = editstring[str]
    until not editstring[str]
    return str:gsub('%c+', '')
end

local function create_default(w2l)
    local default = {}
    local need_build = false
    for _, name in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'txt', 'misc'} do
        local str = loader:mpq_load(w2l.default .. '\\' .. name .. '.ini')
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
    if not default then
        default = create_default(self)
    end
    return default
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
            print('-report|9其他', '没有找到字符串定义:', ('TRIGSTR_%03d'):format(i))
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
    print('-report|7保存到wts中的文本', reason)
    print('-tip', '文本保存在wts中会导致加载速度变慢: ', (text:sub(1, 1000):gsub('\r\n', ' ')))
    if text:find('}', 1, false) then
        print('-report|2警告', '文本中的"}"被修改为了"|"')
        print('-tip', (text:sub(1, 1000):gsub('\r\n', ' ')))
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

setmetatable(mt, mt)
function mt:__index(name)
    if name == 'info' then
        self.info = lni(assert(load_file('info.ini')), 'info')
        return self.info
    end
    local f = io.open('slk\\'..name..'.lua')
    if f then
        f:close()
        self[name] = require('slk.'..name)
        return self[name]
    end
    local f = io.open('other\\'..name..'.lua')
    if f then
        f:close()
        self[name] = require('other.'..name)
        return self[name]
    end
    return nil
end

function mt:set_config(config)
    self.config = config
    self.mpq = self.config.mpq
    if self.config.version == 'Melee' then
        self.agent = self.mpq
        self.default = 'prebuilt\\' .. self.mpq .. '\\Melee'
    else
        self.agent = self.mpq .. '\\Custom_V1'
        self.default = 'prebuilt\\' .. self.mpq .. '\\Custom'
    end
end

function mt:set_messager(prt)
    print = prt
end

mt:set_messager(function() end)

return mt
