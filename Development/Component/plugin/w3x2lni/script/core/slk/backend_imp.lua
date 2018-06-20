local w2l

local function parse_imp(buf)
    local list = {}
    local _, count, index = ('ll'):unpack(buf)
    local name
    for i = 1, count do
        _, name, index = ('c1z'):unpack(buf, index)
        if w2l:file_load('map', name) then
            list[#list+1] = name
        elseif w2l:file_load('map', 'War3mapImported\\' .. name) then
            list[#list+1] = 'War3mapImported\\' .. name
        end
    end
    table.sort(list)
    return list
end

local function convert_imp(list)
    local lines = {}
    lines[1] = '[root]'
    lines[2] = 'import = {'
    for _, name in ipairs(list) do
        lines[#lines+1] = ('%q'):format(name) .. ','
    end
    lines[#lines+1] = '}'
    return table.concat(lines, '\r\n')
end

return function (w2l_, buf)
    w2l = w2l_
    local list = parse_imp(buf)
    local buf = convert_imp(list)
    return buf
end
