local format = string.format
local tonumber = tonumber

local fmt = {
    '%.1f',
    '%.2f',
    '%.3f',
    '%.4f',
    '%.4f',
    '%.5f',
    '%.6f',
    '%.7f',
    '%.8f',
    '%.9f',
    '%.10f',
    '%.11f',
    '%.12f',
    '%.13f',
    '%.14f',
    '%.15f',
    '%.16f',
}

return function (v)
    for i = 1, #fmt do
        local g = format(fmt[i], v)
        if tonumber(g) == v then
            return g
        end
    end
    return format('%.17f', v)
end
