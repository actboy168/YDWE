local wtonumber = require 'w3xparser'.tonumber

local w2l
local marks
local metadata

local function txt_to_type(tp, value)
    if tp == 0 then
        if not value then
            return 0
        end
        return math.floor(wtonumber(value))
    elseif tp == 1 or tp == 2 then
        if not value then
            return 0.0
        end
        return wtonumber(value) + 0.0
    elseif tp == 3 then
        if not value then
            return nil
        end
        return tostring(value)
    end
end

local function add_obj(name, obj, meta)
    if not obj then
        return
    end
    local misc = {
        _id = name,
        _code = name,
        _type = 'misc',
    }
    for key, value in pairs(meta) do
        if value ~= 'misc' and obj[key] then
            if type(obj[key]) == 'table' then
                misc[key] = txt_to_type(value.type, table.concat(obj[key], ','))
            else
                misc[key] = txt_to_type(value.type, obj[key])
            end
        end
    end
    return misc
end

local function convert(data)
    local misc = {}
    for name, meta in pairs(metadata) do
        if meta.type == 'misc' then
            local lname = name:lower()
            misc[name] = add_obj(name, data[lname], meta)
        end
    end
    return misc
end

return function (w2l_, buf)
    if not buf then
        return
    end
    w2l = w2l_
    marks = {}
    metadata = w2l:metadata()
    local misc = convert(w2l:parse_txt(buf, 'war3mapmisc.txt'))
    return misc
end
