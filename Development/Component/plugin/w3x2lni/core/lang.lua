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

local function load(filename)
    local f = io.open(filename, 'rb')
    if not f then
        return nil
    end
    local buf = f:read 'a'
    f:close()
    return buf
end

function mt:load_lng(filename)
    if not lang then
        self:set_lang '${AUTO}'
    end
    local buf = load('locale\\' .. filename .. '.lng')
    return buf
end

function mt:set_lng_file(filename, buf)
    local t = {}
    self[filename] = t
    cache[filename] = true
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
                t[key] = t[key] .. '\n' .. line
            else
                t[key] = line
            end
        end
    end
    return proxy(t)
end

function mt:__index(filename)
    local buf = self:load_lng(filename)
    local t = self:set_lng_file(filename, buf)
    return t
end

function mt:set_lang(lang_)
    if lang_ == '${AUTO}' then
        lang = 'zh-CN'
    else
        lang = lang_
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
