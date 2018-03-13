local hex

local function pack(fmt, ...)
    hex[#hex+1] = fmt:pack(...)
end

local function pack_head()
    pack('c4', 'W3E!')
    pack('l', 11)
end

local function pack_define()
    pack('c1', 'L')
    pack('l', 0)

    pack('l', 2)
    pack('c4c4', 'Ldrt', 'Ldro')

    pack('l', 2)
    pack('c4c4', 'CLdi', 'CLgr')
end

local function pack_size()
    pack('ll', 17, 17)
    pack('ff', 0, 0)
end

local function pack_titles()
    for i = 1, 17*17 do
        pack('c7', '\x00\x20\x00\x60\x00\x10\xf2')
    end
end

return function (w2l_)
    hex = {}

    pack_head()
    pack_define()
    pack_size()
    pack_titles()

    return table.concat(hex)
end
