local lang = require 'lang'
local w2l

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
    return displaytype[o._type], o._id, name:sub(1, 100):gsub('\r\n', ' ')
end

local function get_displayname_by_id(slk, id)
    local o = slk.ability[id]
           or slk.unit[id]
           or slk.buff[id]
           or slk.item[id]
           or slk.destructable[id]
           or slk.doodad[id]
           or slk.upgrade[id]
    if not o then
        return lang.script.UNKNOW, id, '<unknown>'
    end
    return get_displayname(o)
end

local function get_value(t, key)
    local value = t[key]
    if value and type(value) ~= 'table' then
        return value
    end
    local pos = key:find("%d+$")
    if not pos then
        return
    end
    local value = t[key:sub(1, pos-1)]
    if not value or type(value) ~= 'table' then
        return
    end
    local level = tonumber(key:sub(pos))
    if level > t._max_level then
        return 0
    end
    return value[level]
end

local function switch(value)
    return function (mapping)
        if mapping[value] then
            return mapping[value]()
        elseif mapping.default then
            return mapping.default()
        end
    end
end

local function split(str)
    local r = {}
    str:gsub('[^,]+', function (w) r[#r+1] = w end)
    return r
end

local function around_integer(value)
    if math.type(value) ~= 'float' then
        return value
    end
    return math.floor(value + 0.00005)
end

local function computed_value(slk, str, name, field)
    -- TODO: 魔兽计算这个太特殊了，我想我放弃完全模拟了
    local id, key, per = table.unpack(split(str))
    id = id:sub(1, 4)
    local o = slk.ability[id]
           or slk.unit[id]
           or slk.item[id]
           or slk.upgrade[id]
    if not o then
        w2l.messager.report(lang.report.COMPUTED_TEXT_FAILED, 5, ('%s %s %s'):format(get_displayname_by_id(slk, name)), ('%s: <%s>'):format(field, str))
        return
    end
    key = key:lower()
    local res = switch(key) {
        mindmg1 = function ()
            return (get_value(o, 'dmgplus1') or 0) + (get_value(o, 'dice1') or 0)
        end,
        maxdmg1 = function ()
            return (get_value(o, 'dmgplus1') or 0) + (get_value(o, 'dice1') or 0) * (get_value(o, 'sides1') or 0)
        end,
        mindmg2 = function ()
            return (get_value(o, 'dmgplus2') or 0) + (get_value(o, 'dice2') or 0)
        end,
        maxdmg2 = function ()
            return (get_value(o, 'dmgplus2') or 0) + (get_value(o, 'dice2') or 0) * (get_value(o, 'sides2') or 0)
        end,
        realhp = function ()
            return get_value(o, 'hp') or 0
        end,
        default = function ()
            return get_value(o, key)
        end
    }
    if type(res) == 'number' then
        if per == '%' then
            res = res * 100
        end
        return around_integer(res)
    end
    w2l.messager.report(lang.report.COMPUTED_TEXT_FAILED, 5, ('%s %s %s'):format(get_displayname_by_id(slk, id)), ('%s: <%s>'):format(field, str))
    return res
end

local function computed(slk, input, id, key)
    return input:gsub('<([^>]*)>', function(str) return computed_value(slk, str, id, key) end)
end

return function(w2l_, slk)
    w2l = w2l_
    local remove_unuse = w2l.setting.remove_unuse_object
    if slk.ability then
        for _, o in pairs(slk.ability) do
            if remove_unuse and not o._mark then
                goto CONTINUE
            end
            if o.researchubertip then
                o.researchubertip = computed(slk, o.researchubertip, o._id, 'Researchtip')
            end
            if o.ubertip then
                for k, v in pairs(o.ubertip) do
                    o.ubertip[k] = computed(slk, v, o._id, 'Ubertip')
                end
            end
            ::CONTINUE::
        end
    end
    if slk.item then
        for _, o in pairs(slk.item) do
            if remove_unuse and not o._mark then
                goto CONTINUE
            end
            if o.ubertip then
                o.ubertip = computed(slk, o.ubertip, o._id, 'Ubertip')
            end
            if o.description then
                o.description = computed(slk, o.description, o._id, 'Description')
            end
            ::CONTINUE::
        end
    end
    if slk.upgrade then
        for _, o in pairs(slk.upgrade) do
            if remove_unuse and not o._mark then
                goto CONTINUE
            end
            if o.ubertip then
                for k, v in pairs(o.ubertip) do
                    o.ubertip[k] = computed(slk, v, o._id, 'Ubertip')
                end
            end
            ::CONTINUE::
        end
    end
end
