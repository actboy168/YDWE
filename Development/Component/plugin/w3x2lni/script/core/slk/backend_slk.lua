local w3xparser = require 'w3xparser'
local lang = require 'lang'

local table_concat = table.concat
local ipairs = ipairs
local string_char = string.char
local pairs = pairs
local type = type
local table_sort = table.sort
local table_insert = table.insert
local math_floor = math.floor
local wtonumber = w3xparser.tonumber
local math_type = math.type
local os_clock = os.clock
local tonumber = tonumber

local report
local slk
local w2l
local metadata
local keys
local lines
local cx
local cy
local remove_unuse_object
local slk_type
local object
local default
local all_slk
local used

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
    return displaytype[o._type], o._id, (name:sub(1, 100):gsub('\r\n', ' '))
end

local function report_failed(obj, key, tip, info)
    report.n = report.n + 1
    if not report[tip] then
        report[tip] = {}
    end
    if report[tip][obj._id] then
        return
    end
    local type, id, name = get_displayname(obj)
    report[tip][obj._id] = {
        ("%s %s %s"):format(type, id, name),
        ("%s %s"):format(key, info),
    }
end

local slk_keys = {
    ['units\\abilitydata.slk']      = {
        'alias','code','Area1','Area2','Area3','Area4','BuffID1','BuffID2','BuffID3','BuffID4','Cast1','Cast2','Cast3','Cast4','checkDep','Cool1','Cool2','Cool3','Cool4','Cost1','Cost2','Cost3','Cost4','DataA1','DataA2','DataA3','DataA4','DataB1','DataB2','DataB3','DataB4','DataC1','DataC2','DataC3','DataC4','DataD1','DataD2','DataD3','DataD4','DataE1','DataE2','DataE3','DataE4','DataF1','DataF2','DataF3','DataF4','DataG1','DataG2','DataG3','DataG4','DataH1','DataH2','DataH3','DataH4','DataI1','DataI2','DataI3','DataI4','Dur1','Dur2','Dur3','Dur4','EfctID1','EfctID2','EfctID3','EfctID4','HeroDur1','HeroDur2','HeroDur3','HeroDur4','levels','levelSkip','priority','reqLevel','Rng1','Rng2','Rng3','Rng4','targs1','targs2','targs3','targs4','UnitID1','UnitID2','UnitID3','UnitID4',
    },
    ['units\\abilitybuffdata.slk']  = {
        'alias',
    },
    ['units\\destructabledata.slk'] = {
        'DestructableID','armor','cliffHeight','colorB','colorG','colorR','deathSnd','fatLOS','file','fixedRot','flyH','fogRadius','fogVis','HP','lightweight','maxPitch','maxRoll','maxScale','minScale','MMBlue','MMGreen','MMRed','Name','numVar','occH','pathTex','pathTexDeath','portraitmodel','radius','selcircsize','selectable','shadow','showInMM','targType','texFile','texID','tilesetSpecific','useMMColor','walkable',
    },
    ['units\\itemdata.slk']         = {
        'itemID','abilList','armor','class','colorB','colorG','colorR','cooldownID','drop','droppable','file','goldcost','HP','ignoreCD','Level','lumbercost','morph','oldLevel','pawnable','perishable','pickRandom','powerup','prio','scale','sellable','stockMax','stockRegen','stockStart','usable','uses',
    },
    ['units\\upgradedata.slk']      = {
        'upgradeid','base1','base2','base3','base4','class','code1','code2','code3','code4','effect1','effect2','effect3','effect4','global','goldbase','goldmod','inherit','lumberbase','lumbermod','maxlevel','mod1','mod2','mod3','mod4','timebase','timemod', 'used',
    },
    ['units\\unitabilities.slk']    = {
        'unitAbilID','abilList','auto','heroAbilList',
    },
    ['units\\unitbalance.slk']      = {
        'unitBalanceID','AGI','AGIplus','bldtm','bountydice','bountyplus','bountysides','collision','def','defType','defUp','fmade','fused','goldcost','goldRep','HP','INT','INTplus','isbldg','level','lumberbountydice','lumberbountyplus','lumberbountysides','lumbercost','lumberRep','mana0','manaN','maxSpd','minSpd','nbrandom','nsight','preventPlace','Primary','regenHP','regenMana','regenType','reptm','repulse','repulseGroup','repulseParam','repulsePrio','requirePlace','sight','spd','stockMax','stockRegen','stockStart','STR','STRplus','tilesets','type','upgrades',
    },
    ['units\\unitdata.slk']         = {
        'unitID','buffRadius','buffType','canBuildOn','canFlee','canSleep','cargoSize','death','deathType','fatLOS','formation','isBuildOn','moveFloor','moveHeight','movetp','nameCount','orientInterp','pathTex','points','prio','propWin','race','requireWaterRadius','targType','turnRate',
    },
    ['units\\unitui.slk']           = {
        'unitUIID','name','armor','blend','blue','buildingShadow','customTeamColor','elevPts','elevRad','file','fileVerFlags','fogRad','green','hideHeroBar','hideHeroDeathMsg','hideHeroMinimap','hideOnMinimap','maxPitch','maxRoll','modelScale','nbmmIcon','occH','red','run','scale','scaleBull','selCircOnWater','selZ','shadowH','shadowOnWater','shadowW','shadowX','shadowY','teamColor','tilesetSpecific','uberSplat','unitShadow','unitSound','walk',
    },
    ['units\\unitweapons.slk']      = {
        'unitWeapID','acquire','atkType1','atkType2','backSw1','backSw2','castbsw','castpt','cool1','cool2','damageLoss1','damageLoss2','dice1','dice2','dmgplus1','dmgplus2','dmgpt1','dmgpt2','dmgUp1','dmgUp2','Farea1','Farea2','Harea1','Harea2','Hfact1','Hfact2','impactSwimZ','impactZ','launchSwimZ','launchX','launchY','launchZ','minRange','Qarea1','Qarea2','Qfact1','Qfact2','rangeN1','rangeN2','RngBuff1','RngBuff2','showUI1','showUI2','sides1','sides2','spillDist1','spillDist2','spillRadius1','spillRadius2','splashTargs1','splashTargs2','targCount1','targCount2','targs1','targs2','weapsOn','weapTp1','weapTp2','weapType1','weapType2',
    },
    ['doodads\\doodads.slk']        = {
        'doodID','file','Name','doodClass','soundLoop','selSize','defScale','minScale','maxScale','maxPitch','maxRoll','visRadius','walkable','numVar','floats','shadow','showInFog','animInFog','fixedRot','pathTex','showInMM','useMMColor','MMRed','MMGreen','MMBlue','vertR01','vertG01','vertB01','vertR02','vertG02','vertB02','vertR03','vertG03','vertB03','vertR04','vertG04','vertB04','vertR05','vertG05','vertB05','vertR06','vertG06','vertB06','vertR07','vertG07','vertB07','vertR08','vertG08','vertB08','vertR09','vertG09','vertB09','vertR10','vertG10','vertB10',
    },
}

local index = {1, 1, 1, 1}
local strs1 = {}
for c in ('!@#$%^&*()_=+\\|/?><,`~'):gmatch '.' do
    strs1[#strs1+1] = c
end
local strs2 = {}
for c in ('0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'):gmatch '.' do
    strs2[#strs2+1] = c
end
local function find_unused_id()
    if not used then
        used = {}
        for _, data in pairs(all_slk) do
            for id in pairs(data) do
                used[id] = true
            end
        end
    end
    while true do
        local id = strs1[index[1]] .. strs2[index[2]] .. strs2[index[3]] .. strs2[index[4]]
        if not used[id] then
            used[id] = true
            return id
        end
        for i = 4, 1, -1 do
            if i > 1 then
                index[i] = index[i] + 1
                if index[i] > #strs2 then
                    index[i] = 1
                else
                    break
                end
            else
                index[i] = index[i] + 1
                if index[i] > #strs1 then
                    return nil
                end
            end
        end
    end
end

local function add_end()
    lines[#lines+1] = 'E'
end

local function add(x, y, k)
    local strs = {}
    strs[#strs+1] = 'C'
    if x ~= cx then
        cx = x
        strs[#strs+1] = 'X' .. x
    end
    if y ~= cy then
        cy = y
        strs[#strs+1] = 'Y' .. y
    end
    if type(k) == 'string' then
        k = ('"%s"'):format(k:gsub('\r\n', '|n'):gsub('[\r\n]', '|n'))
    elseif math_type(k) == 'float' then
        k = ('%.4f'):format(k):gsub('[0]+$', ''):gsub('%.$', '.0')
    end
    strs[#strs+1] = 'K' .. k
    lines[#lines+1] = table_concat(strs, ';')
end

local function add_values(names, skeys, slk_name)
    local clock = os_clock()
    for y, name in ipairs(names) do
        local obj = slk[name]
        for x, key in ipairs(skeys) do
            local value = obj[key]
            if value then
                add(x, y+1, value)
            elseif slk_name == 'units\\unitabilities.slk' and key == 'auto'
                or slk_name == 'units\\unitbalance.slk' and (key == 'Primary' or key == 'preventPlace' or key == 'requirePlace')
                or slk_name == 'units\\unitui.slk' and key == 'file'
                or slk_name == 'units\\destructabledata.slk' and key == 'texFile'
                or slk_name == 'units\\abilitydata.slk' and (key == 'targs1' or key == 'targs2' or key == 'targs3' or key == 'targs4')
            then
                add(x, y+1, '_')
            elseif slk_name == 'units\\upgradedata.slk' and key == 'used' then
                add(x, y+1, 1)
            end
        end
        if os_clock() - clock > 0.1 then
            clock = os_clock()
            w2l.progress(y / #names)
            w2l.messager.text(lang.script.CONVERT_FILE:format(slk_type, name, y, #names))
        end
    end
end

local function add_title(names)
    for x, name in ipairs(names) do
        add(x, 1, name)
    end
end

local function add_head(names, skeys)
    lines[#lines+1] = 'ID;PWXL;N;E'
    lines[#lines+1] = ('B;X%d;Y%d;D0'):format(#skeys, #names+1)
end

local function get_names()
    local names = {}
    for name in pairs(slk) do
        names[#names+1] = name
    end
    table_sort(names, function(name1, name2)
        if default[name1] and not default[name2] then
            return true
        elseif not default[name1] and default[name2] then
            return false
        else
            return name1 < name2
        end
    end)
    return names
end

local function convert_slk(slk_name)
    local names = get_names()
    local skeys = slk_keys[slk_name]
    add_head(names, skeys)
    add_title(skeys)
    add_values(names, skeys, slk_name)
    add_end()
end

local function to_type(tp, value)
    if tp == 0 then
        if not value or value == 0 then
            return nil
        end
        return math_floor(wtonumber(value))
    elseif tp == 1 or tp == 2 then
        if not value or value == 0 then
            return nil
        end
        return wtonumber(value) + 0.0
    elseif tp == 3 then
        if not value then
            return nil
        end
        if value == '' then
            return nil
        end
        value = tostring(value)
        if not value:match '[^ %-%_]' then
            return nil
        end
        if value:match '^%.[mM][dD][lLxX]$' then
            return nil
        end
        return value
    end
end

local function is_usable_string(str)
    local char = str:sub(1, 1)
    if char == '-' or char == '.' or tonumber(char) then
        return false
    end
    return true
end

local function load_data(meta, obj, key, slk_data, obj_data)
    if not obj[key] then
        return
    end
    local displaykey = meta.field
    local tp = meta.type
    if type(obj[key]) == 'table' then
        local over_level
        obj_data[key] = {}
        if slk_type == 'doodad' then
            for i = 11, #obj[key] do
                if obj[key][i] ~= obj[key][10] then
                    obj_data[key][i] = obj[key][i]
                    over_level = true
                end
            end
            for i = 1, 10 do
                local value = to_type(tp, obj[key][i])
                if value and tp == 3 and not is_usable_string(value) then
                    obj_data[key][i] = value
                    report_failed(obj, displaykey, lang.report.STRING_CAN_CONVERT_NUMBER, value)
                else
                    slk_data[('%s%02d'):format(displaykey, i)] = value
                end
            end
        else
            for i = 5, #obj[key] do
                if obj[key][i] ~= obj[key][4] then
                    obj_data[key][i] = obj[key][i]
                    over_level = true
                end
            end
            for i = 1, 4 do
                local value = to_type(tp, obj[key][i])
                if value and tp == 3 and not is_usable_string(value) then
                    obj_data[key][i] = value
                    report_failed(obj, displaykey, lang.report.STRING_CAN_CONVERT_NUMBER, value)
                else
                    slk_data[displaykey..i] = value
                end
            end
        end
        if over_level then
            report_failed(obj, displaykey, lang.report.DATA_LEVEL_TOO_HIGHT, '')
        end
    else
        local value = to_type(tp, obj[key])
        if value and tp == 3 and not is_usable_string(value) then
            obj_data[key] = value
            report_failed(obj, displaykey, lang.report.STRING_CAN_CONVERT_NUMBER, value)
        else
            slk_data[displaykey] = value
        end
    end
end

local function load_obj(id, obj, slk_name)
    if remove_unuse_object and not obj._mark then
        return nil
    end
    local obj_data = object[id]
    if not obj_data then
        obj_data = {}
        object[id] = obj_data
        obj_data._id     = obj._id
        obj_data._slk    = not obj._keep_obj
        obj_data._code   = obj._code
        obj_data._mark   = obj._mark
        obj_data._parent = obj._parent
        obj_data._keep_obj = obj._keep_obj
    end
    if obj._keep_obj then
        return nil
    end
    if not obj._slk_id and not is_usable_string(obj._id) then
        obj._slk_id = find_unused_id()
        obj_data._slk_id = obj._slk_id
        if slk_type == 'ability' then
            obj_data.name = obj.name
        end
        report_failed(obj, 'id', lang.report.OBJECT_ID_CAN_CONVERT_NUMBER, '')
        if not obj._slk_id then
            return nil
        end
    end
    local slk_data = {}
    slk_data[slk_keys[slk_name][1]] = obj._slk_id or obj._id
    slk_data['code'] = obj._code
    if obj._name then
        if obj._id == obj._parent then
            slk_data['name'] = obj._name
        else
            slk_data['name'] = 'custom_' .. obj._id
        end
    end
    obj._slk = true
    for _, key in ipairs(keys) do
        local meta = metadata[slk_type][key]
        load_data(meta, obj, key, slk_data, obj_data)
    end
    if metadata[obj._code] then
        for key, meta in pairs(metadata[obj._code]) do
            load_data(meta, obj, key, slk_data, obj_data)
        end
    end
    return slk_data
end

local function sort_pairs(t)
    local keys = {}
    for k in pairs(t) do
        keys[#keys+1] = k
    end
    table_sort(keys)
    local i = 0
    return function ()
        i = i + 1
        local k = keys[i]
        return k, t[k]
    end
end

local function load_chunk(chunk, slk_name)
    for id, obj in sort_pairs(chunk) do
        slk[id] = load_obj(id, obj, slk_name)
    end
end

return function(w2l_, type, slk_name, chunk, report_, obj, slk_)
    if not chunk then
        return nil
    end
    slk = {}
    w2l = w2l_
    report = report_
    object = obj
    all_slk = slk_
    cx = nil
    cy = nil
    remove_unuse_object = w2l.setting.remove_unuse_object
    lines = {}
    metadata = w2l:metadata()
    keys = w2l:keydata()[slk_name]
    default = w2l:get_default()[type]
    slk_type = type

    load_chunk(chunk, slk_name)
    convert_slk(slk_name)
    return table_concat(lines, '\r\n')
end
