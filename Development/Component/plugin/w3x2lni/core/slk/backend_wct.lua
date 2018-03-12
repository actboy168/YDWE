local hex
local wct

local function pack(fmt, ...)
    hex[#hex+1] = fmt:pack(...)
end

local function pack_head()
    pack('l', 1)
end

local function pack_custom()
    pack('z', wct.custom.comment)
    if #wct.custom.code > 0 then
        -- 这里的字符串长度算上了'\0'，因此要+1
        pack('l', #wct.custom.code+1)
        pack('z', wct.custom.code)
    else
        pack('l', 0)
    end
end

local function pack_triggers()
    pack('l', #wct.triggers)
    for _, trg in ipairs(wct.triggers) do
        if #trg > 0 then
            -- 这里的字符串长度算上了'\0'，因此要+1
            pack('l', #trg+1)
            pack('z', trg)
        else
            pack('l', 0)
        end
    end
end

return function (w2l_, wct_)
    hex = {}
    wct = wct_

    pack_head()
    pack_custom()
    pack_triggers()
    
    return table.concat(hex)
end
