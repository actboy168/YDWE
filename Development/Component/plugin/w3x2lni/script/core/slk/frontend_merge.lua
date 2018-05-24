local lang = require 'lang'
local w2l
local is_remove_exceeds_level
local metadata

local pairs = pairs
local type = type
local assert = assert

local function maxindex(t)
    local i = 0
    for k in pairs(t) do
        if k > i then
            i = k
        end
    end
    return i
end

local function fill_and_copy(a, lv)
    local c = {}
    if #a < lv then
        for i = 1, #a do
            c[i] = a[i]
        end
        for i = #a+1, lv do
            c[i] = a[#a]
        end
    else
        for i = 1, lv do
            c[i] = a[i]
        end
    end
    if not is_remove_exceeds_level then
        local maxlv = maxindex(a)
        if maxlv > lv then
            for i = lv+1, maxlv do
                c[i] = a[i] or a[#a]
            end
        end
    end
    return c
end

local function fill_and_merge(a, b, lv, meta)
    local c = {}
    if #a < lv then
        for i = 1, #a do
            c[i] = b[i] or a[i]
        end
        if meta and meta.appendindex then
            for i = #a+1, lv do
                c[i] = b[i] or ''
            end
        elseif not meta or meta.profile then
            local maxlv = maxindex(b)
            for i = #a+1, lv do
                if i > maxlv then
                    c[i] = b[i] or c[i-1]
                else
                    c[i] = b[i] or ''
                end
            end
        else
            for i = #a+1, lv do
                c[i] = b[i] or a[#a]
            end
        end
    else
        for i = 1, lv do
            c[i] = b[i] or a[i] 
        end
    end
    if not is_remove_exceeds_level then
        local maxlv = maxindex(b)
        if maxlv > lv then
            for i = lv+1, maxlv do
                c[i] = b[i] or a[#a]
            end
        end
    end
    return c
end

local function copy_obj(a, b)
    local c = {}
    local lv = b._max_level or a._max_level
    if b._code and a._code ~= b._code then
        w2l.messager.report(lang.report.INVALID_OBJECT_DATA, 6, lang.report.ABILITY_CODE_ERROR:format(b._id, b._code), lang.report.DEFAULT_IS:format(a._code))
        return nil
    end
    for k, v in pairs(a) do
        if b[k] then
            if type(v) == 'table' then
                c[k] = fill_and_merge(v, b[k], lv, metadata[a._code] and metadata[a._code][k] or metadata[a._type] and metadata[a._type][k])
            else
                c[k] = b[k]
            end
            b[k] = nil
        else
            if type(v) == 'table' then
                c[k] = fill_and_copy(v, lv)
            else
                c[k] = v
            end
        end
    end
    for k, v in pairs(b) do
        -- 不应该会有等级数据
        assert(type(v) ~= 'table')
        c[k] = v
    end
    return c
end

local function fill_obj(a)
    local c = {}
    local lv = a._max_level
    for k, v in pairs(a) do
        if type(v) == 'table' then
            c[k] = fill_and_copy(v, lv)
        else
            c[k] = v
        end
    end
    return c
end

return function (w2l_, type, data, objs)
    w2l = w2l_
    if type == 'txt' then
        is_remove_exceeds_level = false
    else
        is_remove_exceeds_level = true
    end
    metadata = w2l:metadata()
    local template = {}
    local result = {}
    for name, obj in pairs(objs) do
        local source = data[name]
        if source then
            template[name] = source
            data[name] = nil
        else
            source = template[obj._parent] or data[obj._parent]
        end
        if source then
            result[name] = copy_obj(source, obj)
        else
            assert(type == 'txt')
            result[name] = obj
        end
    end
    for name, obj in pairs(data) do
        result[name] = fill_obj(obj)
    end
    return result
end
