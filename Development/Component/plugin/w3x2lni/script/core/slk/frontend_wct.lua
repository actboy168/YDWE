local lang = require 'lang'
local wct
local unpack_index
local chunk

local function unpack(fmt)
    local result
    result, unpack_index = fmt:unpack(wct, unpack_index)
    return result
end

local function read_head()
    local ver = unpack 'l'
    assert(ver == 1, lang.script.UNSUPPORTED_WCT)
end

local function read_custom()
    chunk.custom = {}
    chunk.custom.comment = unpack 'z'
    local size = unpack 'l'
    if size == 0 then
        chunk.custom.code = ''
    else
        chunk.custom.code = unpack 'z'
    end
end

local function read_triggers()
    chunk.triggers = {}
    local count = unpack 'l'
    for i = 1, count do
        local size = unpack 'l'
        if size == 0 then
            chunk.triggers[i] = ''
        else
            chunk.triggers[i] = unpack 'z'
        end
    end
end

return function (w2l, wct_)
    wct = wct_
    unpack_index = 1
    chunk = {}

    read_head()
    read_custom()
    read_triggers()
    
    return chunk
end
