local lang = require 'lang'
local table_insert = table.insert
local table_sort = table.sort
local math_type = math.type
local table_concat = table.concat
local string_char = string.char
local type = type
local os_clock = os.clock

local w2l
local metadata
local remove_unuse_object
local ttype
local str

local function get_len(tbl)
    local n = 0
    for k in pairs(tbl) do
        if type(k) == 'number' and k > n then
            n = k
        end
    end
    return n
end

local function format_value(tp, value)
    if value == nil then
        return 'nil'
    end
    if tp == 0 then
        return ('%d'):format(math.floor(value))
    elseif tp == 1 or tp == 2 then
        return ('%.4f'):format(value)
    elseif tp == 3 then
        value = w2l:get_editstring(value)
        if value:match '[\n\r]' then
            return ('[=[\r\n%s]=]'):format(value)
        else
            return ('%q'):format(value)
        end
    end
    return 'nil'
end

local function write(format, ...)
    str[#str+1] = format:format(...)
end

local function write_data(meta, data, lines)
    local len
    local key = meta.field
    if type(data) == 'table' then
        len = get_len(data)
        if len == 0 then
            return
        end
    end
    if key:match '[^%w%_]' then
        key = ('%q'):format(key)
    end
    if meta.displayname then
        local comment = w2l:get_editstring(meta.displayname)
        lines[#lines+1] = {'-- %s', comment:gsub('^%s*(.-)%s*$', '%1')}
    end
    if not len then
        lines[#lines+1] = {'%s = %s', key, format_value(meta.type, data)}
        return
    end
    if len <= 1 then
        lines[#lines+1] = {'%s = %s', key, format_value(meta.type, data[1])}
        return
    end

    local values = {}
    local is_string
    for i = 1, len do
        if type(data[i]) == 'string' then
            is_string = true
        end
        if len >= 10 then
            values[i] = ('%d = %s'):format(i, format_value(meta.type, data[i]))
        else
            values[i] = format_value(meta.type, data[i])
        end
    end

    if is_string or len >= 10 then
        lines[#lines+1] = {'%s = {\r\n%s,\r\n}', key, table_concat(values, ',\r\n')}
        return
    end
    
    lines[#lines+1] = {'%s = {%s}', key, table_concat(values, ', ')}
end

local function write_obj(id, obj)
    local metas = {}
    local datas = {}
    local haskey = {}
    if metadata[obj._code] then
        for key, meta in pairs(metadata[obj._code]) do
            local data = obj[key]
            if data then
                metas[#metas+1] = meta
                datas[meta] = data
            end
            haskey[key] = true
        end
    end
    if metadata[ttype] then
        for key, meta in pairs(metadata[ttype]) do
            if not haskey[key] then
                local data = obj[key]
                if data then
                    metas[#metas+1] = meta
                    datas[meta] = data
                end
            end
        end
    end
    table_sort(metas, function(meta1, meta2)
        return meta1.field < meta2.field
    end)
    local lines = {}
    for _, meta in ipairs(metas) do
        write_data(meta, datas[meta], lines)
    end
    if #lines == 0 and id == obj._parent then
        return
    end
    if id:match '[^%w%_]' then
        write('[%q]', id)
    else
        write('[%s]', id)
    end
    if obj._parent then
        write('%s = %q', '_parent', obj._parent)
    end
    for i = 1, #lines do
        write(table.unpack(lines[i]))
    end
    write ''
end

local function write_table(slk)
    local list = {}
    for id, obj in pairs(slk) do
        if not remove_unuse_object or obj._mark then
            list[#list+1] = id
        end
    end
    table_sort(list, function(a, b)
        local is_origin_a = a == slk[a]._parent
        local is_origin_b = b == slk[b]._parent
        if is_origin_a and not is_origin_b then
            return true
        end
        if not is_origin_a and is_origin_b then
            return false
        end
        return a < b
    end)
    local clock = os_clock()
    for i = 1, #list do
        local obj = slk[list[i]]
        write_obj(list[i], obj)
        if os_clock() - clock >= 0.1 then
            clock = os_clock()
            w2l.messager.text(lang.script.CONVERT_FILE:format(ttype, obj._id, i, #list))
            w2l.progress(i / #list)
        end
    end
end

return function (w2l_, type, slk)
    if not slk then
        return
    end
    w2l = w2l_
    metadata = w2l:metadata()
    remove_unuse_object = w2l.setting.remove_unuse_object
    ttype = type
    str = {}
    write_table(slk)
    return #str > 0 and table_concat(str, '\r\n')
end
