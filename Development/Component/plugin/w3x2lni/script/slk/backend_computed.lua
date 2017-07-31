local displaytype = {
    unit = '单位',
    ability = '技能',
    item = '物品',
    buff = '魔法效果',
    upgrade = '科技',
    doodad = '装饰物',
    destructable = '可破坏物',
}

local function get_displayname(o)
    local name
    if o._type == 'buff' then
        name = o.bufftip or o.editorname or ''
    elseif o._type == 'upgrade' then
        name = o.name[1] or ''
    else
        name = o.name or ''
    end
    return displaytype[o._type], o._id, (name:sub(1, 100):gsub('\r\n', ' '))
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
        return '未知', id, '<unknown>'
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

local function computed_value(slk, str, name, field)
    -- TODO: 魔兽计算这个太特殊了，我想我放弃完全模拟了
    local id, key, per = table.unpack(split(str))
    id = id:sub(1, 4)
    local o = slk.ability[id]
           or slk.unit[id]
           or slk.item[id]
           or slk.upgrade[id]
    if not o then
        message('-report|5公式计算失败', get_displayname_by_id(slk, name))
        message('-tip', ('%s: <%s>'):format(field, str))
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
        return math.floor(res)
    end
    message('-report|5公式计算失败', get_displayname_by_id(slk, id))
    message('-tip', ('%s: <%s>'):format(field, str))
    return res
end

local function computed(slk, input, id, key)
    return input:gsub('<([^>]*)>', function(str) return computed_value(slk, str, id, key) end)
end

return function(w2l, slk)
    local remove_unuse = w2l.config.remove_unuse_object
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
