local progress = require 'progress'

local pairs = pairs

local keydata
local is_remove_same
local w2l
local default
local metadata

local function sortpairs(t)
    local sort = {}
    for k, v in pairs(t) do
        sort[#sort+1] = {k, v}
    end
    table.sort(sort, function (a, b)
        return a[1] < b[1]
    end)
    local n = 1
    return function()
        local v = sort[n]
        if not v then
            return
        end
        n = n + 1
        return v[1], v[2]
    end
end

local function remove_same_as_slk(key, data, default, obj, ttype)
    local dest = default[key]
    if type(dest) == 'table' then
        local new_data = {}
        for i = 1, #data do
            local default
            if i > #dest then
                default = dest[#dest]
            else
                default = dest[i]
            end
            if data[i] ~= default then
                new_data[i] = data[i]
            end
        end
        if not next(new_data) then
            obj[key] = new_data
            return
        end
        if is_remove_same then
            obj[key] = new_data
        end
    else
        if data == dest then
            obj[key] = nil
        end
    end
end

local function remove_same_as_txt(key, data, default, obj, ttype)
    local dest = default[key]
    if type(dest) == 'table' then
        local new_data = {}
        local last
        for i = 1, #data do
            local default
            if i > #dest then
                default = last
            else
                default = dest[i]
            end
            if data[i] ~= default then
                new_data[i] = data[i]
            end
            last = data[i]
        end
        if not next(new_data) then
            obj[key] = new_data
            return
        end
        if is_remove_same then
            obj[key] = new_data
        end
    else
        if data == dest then
            obj[key] = nil
        end
    end
end

local function clean_obj(name, obj, type, default)
    local parent = obj._parent
    local max_level = obj._max_level
    local default = default[parent]
    for key, meta in pairs(metadata[type]) do
        local data = obj[key]
        if data then
            if meta.profile then
                remove_same_as_txt(key, data, default, obj, type)
            else
                remove_same_as_slk(key, data, default, obj, type)
            end 
        end
    end
    if metadata[obj._code] then
        for key, meta in pairs(metadata[obj._code]) do
            local data = obj[key]
            if data then
                if meta.profile then
                    remove_same_as_txt(key, data, default, obj, type)
                else
                    remove_same_as_slk(key, data, default, obj, type)
                end 
            end
        end
    end
end

local function clean_objs(type, t)
    for id, obj in sortpairs(t) do
        clean_obj(id, obj, type, default[type])
    end
end

local function clean_misc(type, t)
    for _, name in ipairs {'FontHeights', 'InfoPanel', 'Misc', 'PingColor', 'QuestIndicatorTimeout', 'SelectionCircle'} do
        clean_obj(id, t[name], type, default[type])
    end
end

return function (w2l_, slk)
    w2l = w2l_
    keydata = w2l:keydata()
    default = w2l:get_default()
    is_remove_same = w2l.config.remove_same
    metadata = w2l:metadata()
    if w2l.config.target_format == 'slk' then
        if not w2l.config.slk_doodad then
            local type = 'doodad'
            clean_objs(type, slk[type])
            progress(0.5)
        end
    else
        for i, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable'} do
            clean_objs(type, slk[type])
            progress(i / 8)
        end
    end
    local type = 'misc'
    clean_misc(type, slk[type])
    progress(1)
end
