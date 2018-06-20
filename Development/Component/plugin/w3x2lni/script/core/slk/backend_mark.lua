local lang = require 'lang'
local w2l
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
    misc = {
        'FontHeights',
        'InfoPanel',
        'Misc',
        'PingColor',
        'QuestIndicatorTimeout',
        'SelectionCircle',
        'HERO',
    }
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
    -- 生命恢复光环
    Aoar = { 'Boar', 'buff' },
    -- 魔法恢复光环
    Aarm = { 'Barm', 'buff' },
}

local slk
local buffmap
local search
local mark_known_type
local report_once = {}
local report_cache = {}
local current_root = {'', '%s%s'}

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
    report_cache[#report_cache+1] = {('%s \'%s\''):format(type, id), format_marktip(slk, current_root)}
end

local function mark_value(slk, type, value, nosearch)
    if std_type(value) == 'string' then
        for _, name in ipairs(split(value)) do
            if not mark_known_type(slk, type, name, nosearch) then
                report(lang.report.DIDNT_FIND_OBJECT_WHEN_SIMPLIFY, name)
            end
        end
    else
        if not mark_known_type(slk, type, value, nosearch) then
            report(lang.report.DIDNT_FIND_OBJECT_WHEN_SIMPLIFY, value)
        end
    end
end

local function mark_list(slk, o, list)
    if not list then
        return
    end
    for key, type in pairs(list) do
        local nosearch = (key == 'upgrades')
        local value = o[key]
        if not value then
        elseif std_type(value) == 'table' then
            for _, name in ipairs(value) do
                mark_value(slk, type, name, nosearch)
            end
        else
            mark_value(slk, type, value, nosearch)
        end
    end
end

local function mark_known_type2(slk, type, name, nosearch)
    local o = slk[type][name]
    if not o then
        local o = slk.txt[name:lower()]
        if o then
            o._mark = current_root
            report_cache[#report_cache+1] = {lang.report.USE_UNCLASSIFIED_OBJECT:format(name:lower()), lang.report.EXPECTATION_CLASSIFICATION:format(type)}
            return true
        end
        return false
    end
    if not o._mark then
        o._mark = current_root
    end
    if not nosearch and not o._mark_child then
        o._mark_child = true
        mark_list(slk, o, search[type])
        if o._code then
            mark_list(slk, o, search[o._code])
            local marklist = mustmark[o._code]
            if marklist then
                if not mark_known_type(slk, marklist[2], marklist[1]) then
                    report(lang.report.DIDNT_FIND_OBJECT_WHEN_SIMPLIFY, marklist[1])
                end
            end
        end
    end
    return true
end

function mark_known_type(slk, type, name, nosearch)
    if type == 'buff' then
        local m = buffmap[name:lower()]
        if m then
            for _, name in ipairs(m) do
                mark_known_type2(slk, type, name, nosearch)
            end
            return true
        end
        return false
    elseif type == 'upgrade,unit' then
        return mark_known_type2(slk, 'unit', name, true)
            or mark_known_type2(slk, 'upgrade', name, true)
            or mark_known_type2(slk, 'misc', name, true)
    else
        return mark_known_type2(slk, type, name, nosearch)
    end
end

local function mark_mustuse(slk)
    for type, list in pairs(mustuse) do
        for _, name in ipairs(list) do
            current_root = {name, lang.report.REFERENCE_BY_MUST_RETAIN}
            if not mark_known_type(slk, type, name) then
                report(lang.report.DIDNT_FIND_OBJECT_WHEN_SIMPLIFY, name)
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
    if not flag then
        return
    end
    if list then
        for name in pairs(list) do
            current_root = {name, lang.report.REFERENCE_BY_JASS_NAME}
            mark(slk, name)
        end
    end
    if flag.building or flag.creeps then
        local maptile = slk.w3i and slk.w3i[lang.w3i.MAP_INFO][lang.w3i.MAP_MAIN_GROUND] or '*'
        for _, obj in pairs(slk.unit) do
            if obj.race == 'creeps' and obj.tilesets and (obj.tilesets == '*' or obj.tilesets:find(maptile)) then
                -- 随机建筑
                if flag.building and obj.isbldg == 1 and obj.nbrandom == 1 then
                    current_root = {obj._id, lang.report.REFERENCE_BY_CREEP_BUILDING}
                    mark_known_type(slk, 'unit', obj._id)
                end
                -- 随机单位
                if flag.creeps and obj.isbldg == 0 and obj.special == 0 then
                    current_root = {obj._id, lang.report.REFERENCE_BY_CREEP_UNIT}
                    mark_known_type(slk, 'unit', obj._id)
                end
            end
        end
    end
    if flag.item then
        for _, obj in pairs(slk.item) do
            if obj.pickrandom == 1 then
                current_root = {obj._id, lang.report.REFERENCE_BY_RANDOM_ITEM}
                mark_known_type(slk, 'item', obj._id)
            end
        end
    end
end

local function mark_marketplace(slk, flag)
    if not flag then
        return
    end
    if not flag.marketplace or flag.item then
        return
    end
    for _, obj in pairs(slk.unit) do
        -- 是否使用了市场
        if obj._mark and obj._name == 'marketplace' then
            search_marketplace = true
            report_cache[#report_cache+1] = {lang.report.RETAIN_MARKET, lang.report.RETAIN_MARKET_HINT:format(obj.name, obj._id)}
            for _, obj in pairs(slk.item) do
                if obj.pickrandom == 1 and obj.sellable == 1 then
                    current_root = {obj._id, lang.report.REFERENCE_BY_MARKET_ITEM}
                    mark_known_type(slk, 'item', obj._id)
                end
            end
            break
        end
    end
end

local function mark_doo(w2l, slk)
    local destructable, doodad = w2l:backend_searchdoo()
    if not destructable then
        return
    end
    for name in pairs(destructable) do
        current_root = {name, lang.report.REFERENCE_BY_PLACING}
        if not mark_known_type(slk, 'destructable', name) then
            mark_known_type(slk, 'doodad', name)
        end
    end
    for name in pairs(doodad) do
        current_root = {name, lang.report.REFERENCE_BY_PLACING}
        mark_known_type(slk, 'doodad', name)
    end
end

local function mark_lua(w2l, slk)
    local list = w2l:call_plugin('on_mark')
    if type(list) ~= 'table' then
        return
    end
    for name in pairs(list) do
        current_root = {name, lang.report.REFERENCE_BY_PLUGIN}
        mark(slk, name)
    end
end

return function(w2l_, slk_)
    w2l = w2l_
    slk = slk_
    if not search then
        search = w2l:parse_lni(w2l:data_load('prebuilt\\search.ini'))
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
    local jasslist, jassflag = w2l:backend_searchjass()
    mark_mustuse(slk)
    mark_jass(slk, jasslist, jassflag)
    mark_doo(w2l, slk)
    mark_lua(w2l, slk)
    mark_marketplace(slk, jassflag)
    if #report_cache > 0 then
        w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, 'TOTAL:' .. #report_cache)
        for _, rep in ipairs(report_cache) do
            w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, rep[1], rep[2])
        end
        w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, '-------------------------------------------')
    end
end
