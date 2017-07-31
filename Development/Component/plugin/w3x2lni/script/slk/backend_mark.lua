local std_type = type
local mustuse =  {
    ability = {
        -- 无敌
        'Avul',
        -- 范围性攻击伤害
        'Adda','Amnz',
        -- 警报
        'Aalr',
        -- 攻击
        'Aatk',
        -- 建造
        'ANbu','AHbu','AObu','AEbu','AUbu','AGbu',
        -- 地洞探测
        'Abdt',
        -- 送回黄金
        'Argd',
        -- 英雄
        'AHer',
        -- 复活英雄
        'Arev',
        -- 集结
        'ARal',
        -- 睡眠
        'ACsp',
        -- 装载
        'Sloa',
        -- 虚无
        'Aetl',
        -- 移动
        'Amov',
        -- 开火
        'Afir','Afih','Afio','Afin','Afiu'
    },
    buff = {
        'BPSE','BSTN','Btlf','Bdet',
        'Bvul','Bspe','Bfro','Bsha',
        'Btrv','Xbdt','Xbli','Xdis', 
        -- 建筑物伤害
        'Xfhs','Xfhm','Xfhl',
        'Xfos','Xfom','Xfol',
        'Xfns','Xfnm','Xfnl',
        'Xfus','Xfum','Xful',
    },
}

-- 目前是技能专属
local mustmark = {
    -- 牺牲深渊 => 阴影
    Asac = { 'ushd', 'unit' },
    Alam = { 'ushd', 'unit' },
    -- 蜘蛛攻击
    Aspa = { 'Bspa', 'buff' },
    -- 战斗号召
    Amil = { 'Bmil', 'buff' },
    -- 天神下凡
    AHav = { 'BHav', 'buff' },
    -- 火凤凰变形
    Aphx = { 'Bphx', 'buff' },
    -- 凤凰火焰
    Apxf = { 'Bpxf', 'buff' },
    -- 反召唤建筑
    Auns = { 'Buns', 'buff' },
    -- 静止陷阱
    Asta = { 'Bstt', 'buff' },
    -- 运输船保持原位
    Achd = { 'Bchd', 'buff' },
}

local slk
local buffmap
local search
local mark_known_type
local mark_known_type_no_child
local report_once = {}
local current_root = {'', '%s%s'}

local function get_displayname(o)
    local name
    if o._type == 'buff' then
        name = o.bufftip or o.editorname or ''
    elseif o._type == 'upgrade' then
        name = o.name[1] or ''
    else
        name = o.name or ''
    end
    return o._id, (name:sub(1, 100):gsub('\r\n', ' '))
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
        return id, '<unknown>'
    end
    return get_displayname(o)
end

local function format_marktip(slk, marktip)
    return marktip[2]:format(get_displayname_by_id(slk, marktip[1]))
end

local function split(str)
    local r = {}
    str:gsub('[^,]+', function (w) r[#r+1] = w end)
    return r
end

local function report(type, id)
    if not report_once[type] then
        report_once[type] = {}
    end
    if report_once[type][id] then
        return
    end
    report_once[type][id] = true
    message('-report|4简化', type, id)
    message('-tip', format_marktip(slk, current_root))
end

local function mark_value(slk, type, value)
    if type == 'upgrade,unit' then
        if std_type(value) == 'string' then
            for _, name in ipairs(split(value)) do
                if not mark_known_type_no_child(slk, 'unit', name) then
                    if not mark_known_type_no_child(slk, 'upgrade', name) then
                        if not mark_known_type_no_child(slk, 'misc', name) then
                            report('简化时没有找到对象:', name)
                        end
                    end
                end
            end
        else
            if not mark_known_type_no_child(slk, 'unit', value) then
                if not mark_known_type_no_child(slk, 'upgrade', value) then
                    if not mark_known_type_no_child(slk, 'misc', value) then
                        report('简化时没有找到对象:', value)
                    end
                end
            end
        end
        return
    end
    if std_type(value) == 'string' then
        for _, name in ipairs(split(value)) do
            if not mark_known_type(slk, type, name) then
                report('简化时没有找到对象:', name)
            end
        end
    else
        if not mark_known_type(slk, type, value) then
            report('简化时没有找到对象:', value)
        end
    end
end

local function mark_list(slk, o, list)
    if not list then
        return
    end
    for key, type in pairs(list) do
        local value = o[key]
        if not value then
        elseif std_type(value) == 'table' then
            for _, name in ipairs(value) do
                mark_value(slk, type, name)
            end
        else
            mark_value(slk, type, value)
        end
    end
end

function mark_known_type_no_child(slk, type, name)
    local o = slk[type][name]
    if not o then
        local o = slk.txt[name:lower()]
        if o then
            o._mark = current_root
            report('引用未分类对象: ', ('%s 期望分类：%s'):format(name:lower(), type))
            return true
        end
        return false
    end
    if o._mark then
        return true
    end
    o._mark = current_root
    return true
end

local function mark_known_type2(slk, type, name)
    local o = slk[type][name]
    if not o then
        local o = slk.txt[name:lower()]
        if o then
            o._mark = current_root
            report('引用未分类对象: ', ('%s 期望分类：%s'):format(name:lower(), type))
            return true
        end
        return false
    end
    if not o._mark then
        o._mark = current_root
    end
    if o._mark_child then
        return true
    end
    o._mark_child = true
    mark_list(slk, o, search[type])
    if o._code then
        mark_list(slk, o, search[o._code])
        local marklist = mustmark[o._code]
        if marklist then
            if not mark_known_type(slk, marklist[2], marklist[1]) then
                report('简化时没有找到对象:', marklist[1])
            end
        end
    end
    return true
end

function mark_known_type(slk, type, name)
    if type == 'buff' then
        local m = buffmap[name:lower()]
        if m then
            for _, name in ipairs(m) do
                mark_known_type2(slk, type, name)
            end
            return true
        end
        return false
    else
        return mark_known_type2(slk, type, name)
    end
end

local function mark_mustuse(slk)
    for type, list in pairs(mustuse) do
        for _, name in ipairs(list) do
            current_root = {name, "必须保留的'%s'[%s]引用了它"}
            if not mark_known_type(slk, type, name) then
                report('简化时没有找到对象:', name)
            end
        end
    end
end

local function mark(slk, name)
    mark_known_type(slk, 'ability', name)
    mark_known_type(slk, 'unit', name)
    mark_known_type(slk, 'buff', name)
    mark_known_type(slk, 'item', name)
    mark_known_type(slk, 'destructable', name)
    mark_known_type(slk, 'doodad', name)
    mark_known_type(slk, 'upgrade', name)
end

local function mark_jass(slk, list, flag)
    if list then
        for name in pairs(list) do
            current_root = {name, "脚本里的'%s'[%s]引用了它"}
            mark(slk, name)
        end
    end
    local maptile = slk.w3i and slk.w3i['地形']['地形类型'] or '*'
    for _, obj in pairs(slk.unit) do
        -- 随机建筑
        if flag.building and obj.isbldg == 1 and obj.nbrandom == 1 then
            if obj.race == 'creeps' and obj.tilesets and (obj.tilesets == '*' or obj.tilesets:find(maptile)) then
                current_root = {obj._id, "保留的野怪建筑'%s'[%s]引用了它"}
                mark_known_type(slk, 'unit', obj._id)
            end
        end
        -- 随机单位
        if flag.creeps and obj.isbldg == 0 then
            if obj.race == 'creeps' and obj.tilesets and (obj.tilesets == '*' or obj.tilesets:find(maptile)) and obj.special == 0 then
                current_root = {obj._id, "保留的野怪单位'%s'[%s]引用了它"}
                mark_known_type(slk, 'unit', obj._id)
            end
        end
    end
    if flag.item then
        for _, obj in pairs(slk.item) do
            if obj.pickrandom == 1 then
                current_root = {obj._id, "保留的随机物品'%s'[%s]引用了它"}
                mark_known_type(slk, 'item', obj._id)
            end
        end
    end
end

local function mark_marketplace(slk, flag)
    if not flag.marketplace or flag.item then
        return
    end
    for _, obj in pairs(slk.unit) do
        -- 是否使用了市场
        if obj._mark and obj._name == 'marketplace' then
            search_marketplace = true
            message('-report|4简化', '保留市场物品')
            message('-tip', ("使用了市场'%s'[%s]"):format(obj.name, obj._id))
            for _, obj in pairs(slk.item) do
                if obj.pickrandom == 1 and obj.sellable == 1 then
                    current_root = {obj._id, "保留的市场物品'%s'[%s]引用了它"}
                    mark_known_type(slk, 'item', obj._id)
                end
            end
            break
        end
    end
end

local function mark_doo(w2l, archive, slk)
    local destructable, doodad = w2l:backend_searchdoo(archive)
    if not destructable then
        return
    end
    for name in pairs(destructable) do
        current_root = {name, "地图上放置的'%s'[%s]引用了它"}
        if not mark_known_type(slk, 'destructable', name) then
            mark_known_type(slk, 'doodad', name)
        end
    end
    for name in pairs(doodad) do
        current_root = {name, "地图上放置的'%s'[%s]引用了它"}
        mark_known_type(slk, 'doodad', name)
    end
end

local function mark_lua(w2l, archive, slk)
    local buf = archive:get('reference.lua')
    if not buf then
        return
    end
    local env = {
        archive  = archive,
        assert   = assert,
        error    = error,
        ipairs   = ipairs,
        load     = load,
        pairs    = pairs,
        next     = next,
        print    = print,
        select   = select,
        tonumber = tonumber,
        tostring = tostring,
        type     = type,
        pcall    = pcall,
        xpcall   = xpcall,
        math     = math,
        string   = string,
        table    = table,
        utf8     = utf8,
    }
    local f, e = load(buf, 'reference.lua', 't', env)
    if not f then
        report('简化时没有找到对象:', e)
        return
    end
    local suc, list = pcall(f)
    if not suc then
        report('简化时没有找到对象:', list)
        return
    end
    if type(list) ~= 'table' then
        return
    end
    for name in pairs(list) do
        current_root = {name, "reference.lua指定保留的'%s'[%s]引用了它"}
        mark(slk, name)
    end
end

return function(w2l, archive, slk_)
    slk = slk_
    if not search then
        search = w2l:parse_lni(assert(io.load(w2l.defined / 'search.ini')))
    end
    buffmap = {}
    for i in pairs(slk.buff) do
        local li = i:lower()
        local m = buffmap[li]
        if m then
            m[#m+1] = i
        else
            buffmap[li] = {i}
        end
    end
    slk.mustuse = mustuse
    local jasslist, jassflag = w2l:backend_searchjass(archive)
    mark_mustuse(slk)
    mark_jass(slk, jasslist, jassflag)
    mark_doo(w2l, archive, slk)
    mark_lua(w2l, archive, slk)
    mark_marketplace(slk, jassflag)
end
