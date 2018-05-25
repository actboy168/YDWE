local cache = {}
local mt = {}
setmetatable(mt, mt)

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
    return cache[filename]
end

function mt:__index(filename)
    local t = self:set_lng_file(filename, '')
    return t
end

return mt
