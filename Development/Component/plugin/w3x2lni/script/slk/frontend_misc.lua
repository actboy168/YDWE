local w3xparser = require 'w3xparser'

local string_lower = string.lower
local table_concat = table.concat
local wtonumber = w3xparser.tonumber
local math_floor = math.floor

local w2l
local marks

local function to_type(tp, value)
    if tp == 0 then
        if not value then
            return 0
        end
        return math_floor(wtonumber(value))
    elseif tp == 1 or tp == 2 then
        if not value then
            return 0.0
        end
        return wtonumber(value) + 0.0
    elseif tp == 3 then
        if not value then
            return nil
        end
        if value == '' then
            return value
        end
        value = tostring(value)
        if not value:match '[^ %-%_]' then
            return nil
        end
        return value
    end
end

local function add_data(key, meta, misc, obj, slk)
    local name = obj._id
    local lname = string_lower(name)
    local value = misc[lname] and misc[lname][key] or slk.txt[lname] and slk.txt[lname][key]
    if not value then
        return
    end
    if meta['concat'] then
        value = table_concat(value, ',')
    else
        value = to_type(meta.type, value[1])
    end
    obj[key] = value
end

local function add_obj(name, meta, misc, chunk, slk)
     chunk[name] = {
        _id = name,
        _code = name,
        _parent = name,
        _type = 'misc',
    }
    local obj = chunk[name]
    for key, meta in pairs(meta) do
        add_data(key, meta, misc, obj, slk)
    end
    local lname = string_lower(name)
    slk.txt[lname] = nil
    if marks[lname] then
        obj._mark = true
    end
end

local function convert(misc, metadata, miscnames, slk)
    local chunk = {}
    for _, name in ipairs(miscnames.misc_names) do
        local meta = metadata[name]
        add_obj(name, meta, misc, chunk, slk)
    end
    return chunk
end

local function merge_misc_data(misc, map_misc, meta, slk)
    if not misc then
        return
    end
    for k, v in pairs(map_misc) do
        if meta[k].type == 3 then
            for i, str in ipairs(v) do
                v[i] = w2l:load_wts(slk.wts, str)
            end
        end
        misc[k] = v
    end
end

local function merge_misc(misc, txt, map_misc, metadata, miscnames, slk)
    for _, name in ipairs(miscnames.misc_names) do
        local lname = name:lower()
        local v = map_misc[lname]
        if v then
            marks[lname] = true
            merge_misc_data(misc[lname] or txt[lname], v, metadata[name], slk)
        end
    end
end

return function (w2l_, archive, slk)
    w2l = w2l_
    marks = {}
    local metadata = w2l:metadata()
    local miscnames = w2l:miscnames()
    local misc = {}
    for _, name in ipairs {"UI\\MiscData.txt", "Units\\MiscData.txt", "Units\\MiscGame.txt"} do
        local buf = io.load(w2l.mpq / name)
        w2l:parse_txt(buf, name, misc)
    end
    local buf = archive:get('war3mapmisc.txt')
    if buf then
        local map_misc = w2l:parse_txt(buf, 'war3mapmisc.txt')
        merge_misc(misc, slk.txt, map_misc, metadata, miscnames, slk)
    end
    slk.misc = convert(misc, metadata, miscnames, slk)
end
