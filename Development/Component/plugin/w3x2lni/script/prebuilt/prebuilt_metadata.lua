local w3xparser = require 'w3xparser'
local slk = w3xparser.slk

local codemapped
local function get_codemapped(w2l, id)
    if not codemapped then
        codemapped = w2l:parse_lni(io.load(w2l.defined / 'codemapped.ini'))
    end
    return codemapped[id] or id
end

local concat_types = {
    abilCode = false,
    abilityList = true,
    aiBuffer = false,
    armorType = false,
    attackBits = false,
    attackTable = true,
    attackType = false,
    attributeType = false,
    bool = false,
    buffList = true,
    channelFlags = false,
    channelType = false,
    combatSound = false,
    deathType = false,
    defenseTable = true,
    defenseType = false,
    defenseTypeInt = false,
    detectionType = false,
    effectList = true,
    fullFlags = false,
    heroAbilityList = true,
    icon = false,
    int = false,
    intList = true,
    interactionFlags = false,
    itemClass = false,
    itemList = true,
    lightningEffect = false,
    lightningList = true,
    model = false,
    modelList = true,
    morphFlags = false,
    moveType = false,
    orderString = false,
    pathingListPrevent = true,
    pathingListRequire = true,
    pathingTexture = false,
    pickFlags = false,
    real = false,
    regenType = false,
    shadowImage = false,
    shadowTexture = false,
    silenceFlags = false,
    soundLabel = false,
    spellDetail = false,
    stackFlags = false,
    string = false,
    stringList = true,
    targetList = true,
    teamColor = false,
    techList = true,
    tilesetList = true,
    uberSplat = false,
    unitClass = true,
    unitCode = false,
    unitList = true,
    unitRace = false,
    unitSound = false,
    unreal = false,
    unrealList = true,
    upgradeClass = false,
    upgradeCode = false,
    upgradeEffect = false,
    upgradeList = true,
    versionFlags = false,
    weaponType = false,
}

local cant_empty = {
    unam = true,
    utub = true,
    ua1z = true,
    gnam = true,
    aart = true,
    atp1 = true,
}

local typedefine
local function get_typedefine(w2l, type)
    if not typedefine then
        typedefine = w2l:parse_lni(io.load(w2l.defined / 'typedefine.ini'))
    end
    return typedefine[type:lower()] or 3
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

local function stringify2(inf, outf)
    for name, obj in sortpairs(inf) do
        outf[#outf+1] = ('[.%s]'):format(fmtstring(name))
        for k, v in sortpairs(obj) do
            outf[#outf+1] = ('%s = %s'):format(fmtstring(k), v)
        end
    end
end

local function stringify(inf, outf)
    for name, obj in sortpairs(inf) do
        if next(obj) then
            outf[#outf+1] = ('[%s]'):format(fmtstring(name))
            stringify2(obj, outf)
            outf[#outf+1] = ''
        end
    end
end

local function stringify_ex(inf)
    local f = {}
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc'} do
        stringify({[type]=inf[type]}, f)
        inf[type] = nil
    end
    stringify(inf, f)
    return table.concat(f, '\r\n')
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
        if meta['useItem'] == 1 then
            return true
        else
            return false
        end
    end
    return true
end

local characters = {'A','B','C','D','E','F','G','H','I'}

local function parse_id(w2l, metadata, id, meta, type, has_level)
    local key = meta.field
    local num  = meta.data
    local objs = meta.useSpecific or meta.section
    if num and num ~= 0 then
        key = key .. characters[num]
    end
    if meta._has_index then
        key = key .. ':' .. (meta.index + 1)
    end
    local data = {
        ['id'] = id,
        ['key'] = meta.slk == 'Profile' and meta.field:lower() or meta.field,
        ['type'] = get_typedefine(w2l, meta.type),
        ['field'] = key,
        ['appendindex'] = meta.appendIndex == 1 and true or nil,
        ['displayname'] = meta.displayName,
    }
    if concat_types[meta.type] then
        data.concat = true
    end
    if has_level and meta['repeat'] > 0 then
        data['repeat'] = meta['repeat']
    end
    if meta.index == -1 and data.type == 3 and not data.concat then
        data.splite = true
    end
    if num and num ~= 0 then
        data.data = num
    end
    if meta.slk == 'Profile' then
        data.profile = true
    end
    if meta._has_index then
        data.index = meta.index + 1
    end
    if cant_empty[id] then
        data.cantempty = true
    end
    local lkey = key:lower()
    if objs then
        for name in objs:gmatch '%w+' do
            local code = get_codemapped(w2l, name)
            if not metadata[code] then
                metadata[code] = {}
            end
            if metadata[code][lkey] and metadata[code][lkey].id ~= data.id then
                message('ID不同:', 'skill', name, 'code', code)
            end
            metadata[code][lkey] = data
        end
    else
        metadata[type][lkey] = data
    end
end

local function add_user_metadata(meta, type)
    meta['w2lobject'] = {
        ['id'] = 'W2lo',
        ['key'] = 'w2lobject',
        ['type'] = 3,
        ['field'] = 'W2LObject',
        ['profile'] = true,
    }
end

local function add_special(meta, type)
    if type == 'unit' then
        meta['missilespeed:1'].default = '{1500,1500}'
        meta['missilespeed:2'].default = '{1500,1500}'
    end
end

local function create_metadata(w2l, type, metadata)
    metadata[type] = {}
    local has_level = w2l.info.key.max_level[type]
    local tbl = slk(io.load(w2l.mpq / w2l.info.metadata[type]))
    tbl.Ytip = nil
    local has_index = {}
    for k, v in pairs(tbl) do
        -- 进行部分预处理
        local name  = v['field']
        local index = v['index']
        if index and index >= 1 then
            has_index[name] = true
        end
    end
    for k, v in pairs(tbl) do
        local name = v['field']
        if has_index[name] then
            v._has_index = true
        end
    end
    for id, meta in pairs(tbl) do
        if is_enable(meta, type) then
            parse_id(w2l, metadata, id, meta, type, has_level)
        end
    end
    add_user_metadata(metadata[type], type)
    add_special(metadata[type], type)
end

local function copy_code(t, template)
    for name, d in pairs(template) do
        local code = d.code
        local data = t[name]
        if data then
            t[name] = nil
            if t[code] then
                for k, v in pairs(data) do
                    local dest = t[code][k]
                    if dest then
                        if v.id ~= dest.id then
                            message('id不同:', k, 'skill:', name, v.id, 'code:', code, dest.id)
                        end
                    else
                        t[code][k] = v
                    end
                end
            else
                t[code] = {}
                for k, v in pairs(data) do
                    t[code][k] = v
                end
            end
        end
    end
end

return function(w2l)
    local metadata = {}
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc'} do
        create_metadata(w2l, type, metadata)
    end
    io.save(w2l.defined / 'metadata.ini', stringify_ex(metadata))
end
