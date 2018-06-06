local w3xparser = require 'w3xparser'
local lang = require 'lang'

local table_insert = table.insert
local table_sort   = table.sort
local table_concat = table.concat
local string_char  = string.char
local math_type    = math.type
local math_floor   = math.floor
local wtonumber = w3xparser.tonumber
local type = type
local pairs = pairs
local setmetatable = setmetatable
local os_clock = os.clock

local w2l
local has_level
local metadata
local default
local hexs
local wts
local ttype

local displaytype = {
    unit = lang.script.UNIT,
    ability = lang.script.ABILITY,
    item = lang.script.ITEM,
    buff = lang.script.BUFF,
    upgrade = lang.script.UPGRADE,
    doodad = lang.script.DOODAD,
    destructable = lang.script.DESTRUCTABLE,
}

local function get_displayname(o)
    local name
    if o._type == 'buff' then
        name = o.bufftip or o.editorname or ''
    elseif o._type == 'upgrade' then
        name = o.name[1] or ''
    elseif o._type == 'doodad' or o._type == 'destructable' then
        name = w2l:get_editstring(o.name or '')
    else
        name = o.name or ''
    end
    return o._id, (name:sub(1, 100):gsub('\r\n', ' '))
end

local function format_value(value)
    if type(value) == 'table' then
        local tbl = {}
        for i = 1, 4 do
            if value[i] then
                tbl[i] = ('[%d]: %s'):format(i, tostring(value[i]):sub(1, 25):gsub('\r\n', ' '))
            end
        end
        return table_concat(tbl, ' ')
    else
        return tostring(value):sub(1, 100):gsub('\r\n', ' ')
    end
end

local function report(reason, obj, key, tip)
    w2l.messager.report(reason, 6, ('%s %s %s'):format(displaytype[ttype], get_displayname(obj)), ('[%s]: %s'):format(key, format_value(tip)))
end

local function write(format, ...)
    hexs[#hexs+1] = (format):pack(...)
end

local function write_value(meta, level, obj, value)
    local id = meta.id
    local tp = meta.type
    write('c4l', id .. ('\0'):rep(4 - #id), tp)
    if has_level then
        write('l', level)
        write('l', meta.data or 0)
    end
    if tp == 0 then
        if math_type(value) ~= 'integer' then
            value = math_floor(wtonumber(value))
        end
        write('l', value)
    elseif tp == 1 or tp == 2 then
        if type(value) ~= 'number' then
            value = wtonumber(value)
        end
        write('f', value)
    else
        if type(value) ~= 'string' then
            value = ''
        end
        if value:find('\0', 1, true) then
            report(lang.report.INVALID_OBJECT_DATA, obj, id, value)
            value = ''
        end
        if #value > 1023 then
            value = w2l:save_wts(wts, value, lang.script.TEXT_TOO_LONG_IN_OBJ)
        end
        write('z', value)
    end
    write('c4', '\0\0\0\0')
end

local function write_data(key, obj, data, meta)
    if meta['repeat'] then
        if type(data) ~= 'table' then
            data = {data}
        end
    end
    if type(data) == 'table' then
        local max_level = 0
        for level in pairs(data) do
            if level > max_level then
                max_level = level
            end
        end
        for level = 1, max_level do
            if data[level] then
                write_value(meta, level, obj, data[level])
            end
        end
    else
        write_value(meta, 0, obj, data)
    end
end

local function write_object(chunk, name, obj)
    local keys = {}
    local metas = {}
    for key in pairs(obj) do
        if key:sub(1, 1) ~= '_' then
            keys[#keys+1] = key
        end
    end
    local code = obj._code
    if metadata[ttype] then
        for key, meta in pairs(metadata[ttype]) do
            metas[key] = meta
        end
    end
    if metadata[code] then
        for key, meta in pairs(metadata[code]) do
            metas[key] = meta
        end
    end
    table_sort(keys)

    local count = 0
    for _, key in ipairs(keys) do
        local data = obj[key]
        if data then
            if metas[key] then
                if type(data) == 'table' then
                    for _ in pairs(data) do
                        count = count + 1
                    end
                else
                    count = count + 1
                end
            else
                if type(data) == 'table' then
                    if next(data) then
                        report(lang.report.INVALID_OBJECT_DATA, obj, key, obj[key])
                    end
                else
                    report(lang.report.INVALID_OBJECT_DATA, obj, key, obj[key])
                end
            end
        end
    end
    
    local parent = obj._slk_id or obj._parent
    if (name == parent or obj._slk) and not obj._slk_id then
        write('c4', name)
        write('c4', '\0\0\0\0')
    else
        write('c4', parent)
        write('c4', name)
    end
    write('l', count)
    for _, key in ipairs(keys) do
        local data = obj[key]
        if data then
            if metas[key] then
                write_data(key, obj, obj[key], metas[key])
            end
        end
    end
end

local function write_chunk(names, data, n, max)
    local clock = os_clock()
    write('l', #names)
    for i, name in ipairs(names) do
        write_object(data, name, data[name])
        if os_clock() - clock > 0.1 then
            clock = os_clock()
            w2l.progress((i+n) / max)
            w2l.messager.text(lang.script.CONVERT_FILE:format(ttype, data[name]._id, i+n, max))
        end
    end
end

local function write_head()
    write('l', 2)
end

local function is_enable_obj(name, obj, remove_unuse_object)
    if remove_unuse_object and not obj._mark then
        return false
    end
    if #name ~= 4 then
        w2l.messager.report(lang.report.INVALID_OBJECT, 6, ('[%s] %s'):format(name, lang.report.INVALID_OBJECT_ID))
        return false
    end
    if not default[obj._parent] then
        w2l.messager.report(lang.report.INVALID_OBJECT, 6, ('[%s:%s] %s'):format(name, obj._parent, lang.report.INVALID_OBJECT_PARENT))
        return false
    end
    if not obj._slk and obj._id ~= obj._parent then
        return true
    end
    if obj._slk_id then
        return true
    end
    if obj._keep_obj then
        return true
    end
    for key, value in pairs(obj) do
        if key:sub(1, 1) ~= '_' then
            if type(value) == 'table' then
                if next(value) then
                    return true
                end
            else
                return true
            end
        end
    end
    return false
end

local function sort_chunk(chunk, remove_unuse_object)
    local origin = {}
    local user = {}
    for name, obj in pairs(chunk) do
        if is_enable_obj(name, obj, remove_unuse_object) then
            local parent = obj._slk_id or obj._parent
            if (name == parent or obj._slk) and not obj._slk_id then
                origin[#origin+1] = name
            else
                user[#user+1] = name
            end
        end
    end
    table_sort(origin)
    table_sort(user)
    return origin, user
end

return function (w2l_, type, data, wts_)
    w2l = w2l_
    if not data then
        return
    end
    if type == 'misc' then
        return w2l:backend_misc(data)
    end
    wts = wts_
    ttype = type
    has_level = w2l.info.key.max_level[type]
    metadata = w2l:we_metadata()
    default = w2l:get_default()[type]
    
    local origin_id, user_id = sort_chunk(data, w2l.setting.remove_unuse_object)
    local max = #origin_id + #user_id
    if max == 0 then
        return
    end
    hexs = {}
    write_head()
    write_chunk(origin_id, data, 0, max)
    write_chunk(user_id, data, #origin_id, max)
    return table_concat(hexs), report
end
