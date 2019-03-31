require 'filesystem'
require 'utility'
local root = require 'backend.w2l_path'
local cache = {}
local mt = {}
setmetatable(mt, mt)

local lang

local function proxy(t)
    return setmetatable(t, { __index = function (_, k)
        t[k] = k
        return k
    end })
end

local function split(buf)
    local lines = {}
    local start = 1
    while true do
        local pos = buf:find('[\r\n]', start)
        if not pos then
            lines[#lines+1] = buf:sub(start)
            break
        end
        lines[#lines+1] = buf:sub(start, pos-1)
        if buf:sub(pos, pos+1) == '\r\n' then
            start = pos + 2
        else
            start = pos + 1
        end
    end
    return lines
end

function mt:load_lng(filename)
    if not lang then
        local config = require 'share.config'
        self:set_lang(config.global.lang)
    end
    local buf = io.load(root / 'script' / 'locale' / lang / (filename .. '.lng'))
    return buf
end

function mt:set_lng_file(filename, buf)
    local t = {}
    self[filename] = t
    cache[filename] = buf
    if not buf then
        return proxy(t)
    end
    local key
    local lines = split(buf)
    for _, line in ipairs(lines) do
        local str = line:match '^%[(.+)%]$'
        if str then
            key = str
        elseif key then
            if t[key] then
                t[key] = t[key] .. '\r\n' .. line
            else
                t[key] = line
            end
        end
    end
    return proxy(t)
end

function mt:get_lng_file(filename)
    local _ = self[filename]
    return cache[filename]
end

function mt:__index(filename)
    local buf = self:load_lng(filename)
    local t = self:set_lng_file(filename, buf)
    return t
end

function mt:set_lang(lang_)
    if lang_ == '${AUTO}' then
        lang = require 'ffi.language' ()
    else
        lang = lang_
    end
    if not fs.exists(root / 'script' / 'locale' / lang) then
        if lang:sub(1, 2) == 'zh' then
            lang = 'zhCN'
        elseif lang:sub(1, 2) == 'en' then
            lang = 'enUS'
        else
            lang = 'enUS'
        end
    end
    for filename in pairs(cache) do
        self[filename] = nil
        cache[filename] = nil
    end
end

function mt:current_lang(type)
    if not lang then
        self:set_lang '${AUTO}'
    end
    return lang
end

return mt
