local messager = require 'share.messager'
local lang = require 'share.lang'
local key_cache = {}
local function get_key(w2l, type, id)
    if not key_cache[type] then
        local t = {}
        for key, meta in pairs(w2l:metadata()[type]) do
            t[meta.id] = key
        end
        key_cache[type] = t
    end
    return key_cache[type][id]
end

local function get_key2(w2l, type, code, id)
    local key = get_key(w2l, type, id)
    if key ~= nil then
        return key
    end
    for key, meta in pairs(w2l:metadata()[code]) do
        if meta.id == id then
            return key
        end
    end
end

local codemapped
local loader

local enable_type = {
    abilCode = 'ability',
    abilityID = 'ability',
    abilityList = 'ability',
    heroAbilityList = 'ability',
    buffList = 'buff',
    effectList = 'buff',
    unitCode = 'unit',
    unitList = 'unit',
    itemList = 'item',
    techList = 'upgrade,unit',
    upgradeList = 'upgrade',
    upgradeCode = 'upgrade',
}

local function fixsearch(t)
    t.item.cooldownid = nil
    t.unit.auto = nil
    t.unit.dependencyor = nil
    t.unit.reviveat = nil
    -- 复活死尸科技限制单位
    t.Arai.unitid = nil -- ACrd、Arai
    t.AIrd.unitid = nil
    t.Avng.unitid = nil
    -- 地洞战备状态允许单位
    t.Abtl.unitid = nil -- Abtl、Sbtl
    -- 装载允许目标单位
    t.Aloa.unitid = nil -- Aloa、Sloa、Slo2、Slo3
    -- 灵魂保存目标单位
    t.ANsl.unitid = nil
    -- 地洞装载允许目标单位
    t.Achl.unitid = nil
    -- 火山爆发召唤可破坏物
    t.ANvc.unitid = 'destructable'
     -- 战斗号召允许单位
    t.Amil.dataa = nil
    -- 骑乘角鹰兽指定单位类型
    t.Acoa.dataa = nil
    t.Acoh.dataa = nil
    t.Acoi.dataa = nil -- Aco2、Aco3
end

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

local function fmtstring(s)
    if s:find '[^%w_]' then
        return ('%q'):format(s)
    end
    return s
end

local function stringify(f, name, t)
    if next(t) then
        f[#f+1] = ('[%s]'):format(fmtstring(name))
        for k, v in sortpairs(t) do
            f[#f+1] = ('%s = %s'):format(fmtstring(k), v)
        end
        f[#f+1] = ''
    end
end

local function is_enable(meta, type)
    if type == 'unit' then
        if meta.useHero == 1 or meta.useUnit == 1 or meta.useBuilding == 1 or meta.useCreep == 1 then
            return true
        else
            return false
        end
    end
    if type == 'item' then
        if meta.useItem == 1 then
            return true
        else
            return false
        end
    end
    return true
end

local function create_search(w2l, type, search)
    search[type] = {}
    local metadata = w2l:parse_slk(loader('units\\' .. w2l.info.metadata[type]) or loader('doodads\\' .. w2l.info.metadata[type]))
    for id, meta in pairs(metadata) do
        if is_enable(meta, type) then
            local objs = meta.useSpecific or meta.section
            if objs then
                for name in objs:gmatch '%w+' do
                    local code = codemapped[name] or name
                    if not search[code] then
                        search[code] = {}
                    end
                    local key = get_key2(w2l, type, code, id)
                    local vtype = enable_type[meta.type]
                    if search[code][key] and search[code][key] ~= vtype then
                        messager.text(lang.raw.TYPE_CONFLICT, 'skill', name, 'code', code)
                    end
                    if key then
                        search[code][key] = vtype
                    end
                end
            else
                local key = get_key(w2l, type, id)
                if key then
                    search[type][key] = enable_type[meta.type]
                end
            end
        end
    end
end

local function get_codemapped(w2l, loader)
    local template = w2l:parse_slk(loader 'units\\abilitydata.slk')
    local t = {}
    for id, d in pairs(template) do
        t[id] = d.code
    end
    return t
end

return function(w2l, loader_)
    loader = loader_
    codemapped = get_codemapped(w2l, loader)
    messager.text(lang.raw.CREATING .. 'search')
    local search = {}
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable'} do
        create_search(w2l, type, search)
    end
    fixsearch(search)
    local f = {}
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable'} do
        stringify(f, type, search[type])
        search[type] = nil
    end
    for k, v in sortpairs(search) do
        stringify(f, k, v)
    end
    return table.concat(f, '\r\n')
end
