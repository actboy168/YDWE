local w2l

local table_concat = table.concat
local table_sort = table.sort
local next = next
local pairs = pairs

local metadata

local function add_data(name, lkey, meta, obj, data)
    if not obj[lkey] then
        return
    end
    local key = meta.field
    data[key] = obj[lkey]
end

local function add_obj(name, obj, data)
    if w2l.setting.remove_unuse_object and not obj._mark then
        return
    end
    local new_obj = {}

    if metadata[name] then
        for lkey, meta in pairs(metadata[name]) do
            add_data(name, lkey, meta, obj, new_obj)
        end
    end

    if next(new_obj) then
        data[name] = new_obj
    end
end

local function convert(misc)
    local data = {}
    for name, obj in pairs(misc) do
        add_obj(name, obj, data)
    end
    return data
end

local function concat_obj(name, obj, lines)
    local keys = {}
    for key in pairs(obj) do
        keys[#keys+1] = key
    end
    table_sort(keys)

    lines[#lines+1] = '[' .. name .. ']'
    for _, key in ipairs(keys) do
        local value = obj[key]
        lines[#lines+1] = key .. '=' .. value
    end
end

local function concat(misc)
    local lines = {}
    local names = {}
    for name in pairs(misc) do
        names[#names+1] = name
    end
    table_sort(names)

    for _, name in ipairs(names) do
        concat_obj(name, misc[name], lines)
    end

    lines[#lines+1] = ''

    return table_concat(lines, '\r\n')
end

return function(w2l_, misc)
    if not misc then
        return
    end
    w2l = w2l_
    metadata = w2l:metadata()
    local data = convert(misc)
    local buf = concat(data)
    return buf
end
