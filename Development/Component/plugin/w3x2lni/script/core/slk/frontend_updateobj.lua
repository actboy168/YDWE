local lang = require 'lang'
local w2l
local has_level
local metadata
local reports
local reports2

local pairs = pairs
local string_sub = string.sub
local string_find = string.find

local function concat(tbl)
    if type(tbl) ~= 'table' then
        return tostring(tbl)
    end
    local max = 0
    for i, v in pairs(tbl) do
        if i > max then
            max = i
        end
    end
    local strs = {}
    for i = 1, max do
        strs[i] = tostring(tbl[i])
    end
    return table.concat(strs, ',')
end

local function update_data(key, meta, obj, new_obj)
    local id = meta.id
    local value = obj[id]
    if not value then
        return
    end
    obj[id] = nil
    if meta.splite then
        for i, str in pairs(value) do
            local pos = string_find(str, ',', 1, true)
            if pos then
                value[i] = string_sub(str, 1, pos-1)
            end
        end
    end
    if meta['repeat'] then
        new_obj[key] = value
    else
        new_obj[key] = value[1]
    end
end

local function update_obj(name, type, obj, data)
    local parent = obj._parent
    local temp = data[parent]
    if not temp then
        reports2[#reports2+1] = {lang.report.TEMPLATE_NOT_EXISTED:format(type, name, parent)}
        return nil
    end
    local code = temp._code
    local new_obj = {}
    obj._code = code
    for key, meta in pairs(metadata[type]) do
        update_data(key, meta, obj, new_obj)
    end
    if metadata[code] then
        for key, meta in pairs(metadata[code]) do
            update_data(key, meta, obj, new_obj)
        end
    end
    for k, v in pairs(obj) do
        if string_sub(k, 1, 1) == '_' then
            new_obj[k] = v
        else
            reports[#reports+1] = {name, k, concat(v, ','):sub(1, 1000):gsub('\r\n', ' ')}
        end
    end
    if has_level then
        new_obj._max_level = new_obj[has_level]
        if new_obj._max_level == 0 then
            new_obj._max_level = 1
        end
    end
    return new_obj
end

return function (w2l_, type, chunk, data)
    w2l = w2l_
    has_level = w2l.info.key.max_level[type]
    metadata = w2l:metadata()
    reports = {}
    reports2 = {}
    for name, obj in pairs(chunk) do
        chunk[name] = update_obj(name, type, obj, data)
    end
    return reports, reports2
end
