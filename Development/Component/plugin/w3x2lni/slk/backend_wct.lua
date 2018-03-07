local hex
local wct

local function pack(fmt, ...)
    hex[#hex+1] = fmt:pack(...)
end

local function pack_head()
    pack('l', 1)
end

local function pack_custom()
    pack('zl', wct.custom.comment, #wct.custom.code)
    if #wct.custom.code > 0 then
        pack('z', wct.custom.code)
    end
end

local function pack_triggers()
    pack('l', #wct.triggers)
    for _, trg in ipairs(wct.triggers) do
        pack('l', #trg)
        if #trg > 0 then
            pack('z', trg)
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
