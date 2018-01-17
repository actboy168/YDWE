local string_lower = string.lower
local pairs = pairs

local w2l
local default

local function add_obj(type, name, level_key, obj)
    local new_obj = {}
    for key, value in pairs(obj) do
        new_obj[string_lower(key)] = value
    end
    new_obj._id = name
    new_obj._max_level = obj[level_key]
    new_obj._type = type
    new_obj._obj = true
    if not default or not default[new_obj._parent] then
        w2l.force_slk = true
    end
    return new_obj
end

return function (w2l_, type, buf, filename)
    w2l = w2l_
    default = w2l:get_default()[type]
    local lni = w2l:parse_lni(buf, filename)
    local metadata = w2l:metadata()[type]
    local level_key = w2l.info.key.max_level[type]
    if level_key then
        level_key = metadata[level_key].field
    end
    local data = {}
    for name, obj in pairs(lni) do
        data[name] = add_obj(type, name, level_key, obj)
    end
    return data
end
