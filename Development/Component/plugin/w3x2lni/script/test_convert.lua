(function()
    local exepath = package.cpath:sub(1, (package.cpath:find(';') or 0)-6)
    package.path = package.path .. ';' .. exepath .. '..\\script\\?.lua'
end)()

local w3xparser = require 'w3xparser'
local wtonumber = w3xparser.tonumber
require 'filesystem'
local uni = require 'ffi.unicode'
local w2l = require 'w3x2lni'
w2l:initialize()

function message(...)
end

local function fixstring(str)
    local r = {}
    str:gsub('[^,]*', function (w)
        if tonumber(w) then
            r[#r+1] = tostring(tonumber(w))
        else
            if w:sub(-1) == '"' then w = w:sub(1,-2) end
            r[#r+1] = w
        end
    end)
    return table.concat(r, ',')
end

local function sortpairs(t)
    local sort = {}
    for k, v in pairs(t) do
        sort[#sort+1] = {k, v}
    end
    table.sort(sort, function (a, b)
        return a[1]:lower() < b[1]:lower()
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

local function convert_txt(inf, outf)
    local t = w2l:parse_ini(assert(io.load(inf)))
    local str = {}
    for id, o in sortpairs(t) do
        str[#str+1] = ('[%s]'):format(id)
        for k, v in sortpairs(o) do
            if k ~= 'EditorSuffix' and k ~= 'EditorName' then
                if k == 'Buttonpos' or k == 'Researchbuttonpos' then
                    if v:sub(-1) == ',' then
                        v = v:sub(1, -2)
                    end
                end
                str[#str+1] = ('%s=%s'):format(k, fixstring(v))
            end
        end
        str[#str+1] = ''
    end
    io.save(outf, table.concat(str, '\r\n'))
end

local txtmap = {
    ['units\\campaignunitstrings.txt'] = 'units\\humanunitfunc.txt',
    ['units\\campaignabilitystrings.txt'] = 'units\\humanabilityfunc.txt',
    ['units\\campaignupgradestrings.txt'] = 'units\\humanupgradefunc.txt',
    ['units\\itemstrings.txt'] = 'units\\itemfunc.txt',
    ['units\\commonabilitystrings.txt'] = 'units\\neutralabilityfunc.txt',
}

local input = fs.path(uni.a2u(arg[1]))

--fs.remove_all(input / 'units2')
fs.create_directories(input / 'units2')
w2l.info.txt.buff = nil
for type, filelist in pairs(w2l.info.txt) do
    for _, filename in ipairs(filelist) do
        if txtmap[filename] then
            convert_txt(input / txtmap[filename], input / 'units2' / fs.path(filename):filename())
        else
            io.save(input / 'units2' / fs.path(filename):filename(), '')
        end
    end
end

local cache = {}

local slkheader = {
    ['units\\abilitydata.slk']      = 'alias',
    ['units\\abilitybuffdata.slk']  = 'alias',
    ['units\\destructabledata.slk'] = 'DestructableID',
    ['units\\itemdata.slk']         = 'itemID',
    ['units\\upgradedata.slk']      = 'upgradeid',
    ['units\\unitabilities.slk']    = 'unitAbilID',
    ['units\\unitbalance.slk']      = 'unitBalanceID',
    ['units\\unitdata.slk']         = 'unitID',
    ['units\\unitui.slk']           = 'unitUIID',
    ['units\\unitweapons.slk']      = 'unitWeapID',
    ['doodads\\doodads.slk']        = 'doodID',
}

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
        'upgradeid','base1','base2','base3','base4','class','code1','code2','code3','code4','effect1','effect2','effect3','effect4','global','goldbase','goldmod','inherit','lumberbase','lumbermod','maxlevel','mod1','mod2','mod3','mod4','timebase','timemod',
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
        'unitWeapID','acquire','atkType1','atkType2','backSw1','backSw2','castbsw','castpt','cool1','cool2','damageLoss1','damageLoss2','dice1','dice2','dmgUp1','dmgUp2','dmgplus1','dmgplus2','dmgpt1','dmgpt2','Farea1','Farea2','Harea1','Harea2','Hfact1','Hfact2','impactSwimZ','impactZ','launchSwimZ','launchX','launchY','launchZ','minRange','Qarea1','Qarea2','Qfact1','Qfact2','rangeN1','rangeN2','RngBuff1','RngBuff2','showUI1','showUI2','sides1','sides2','spillDist1','spillDist2','spillRadius1','spillRadius2','splashTargs1','splashTargs2','targCount1','targCount2','targs1','targs2','weapsOn','weapTp1','weapTp2','weapType1','weapType2',
    },
    ['doodads\\doodads.slk']        = {'doodID', --todo
    },
}

local abilkey = {'Area', 'BuffID','Cast','Cool','Cost','DataA','DataB','DataC','DataD','DataE','DataF','DataG','DataH','DataI','Dur','EfctID','HeroDur','Rng','UnitID','targs'}

local metadata
local keydata1
local keydata2
local firstkey
local function get_key_type(key, code)
    if firstkey == key then
        return 3
    end
    if 'name' == key then
        return 3
    end
    if 'code' == key then
        return 3
    end
    local id = keydata2[key]
    if not id then
        key = key:sub(1, key:find("%d+$")-1) 
        id = keydata2[key] or keydata1[code][key]
    end
    if not id then
        return 4
    end
    return w2l:get_id_type(metadata[id].type)
end

local function to_type(tp, value)
    if tp == 0 then
        if not value or value == 0 then
            return nil
        end
        return math.floor(wtonumber(value))
    elseif tp == 1 or tp == 2 then
        if not value or value == 0 then
            return nil
        end
        return wtonumber(value) + 0.0
    elseif tp == 3 then
        if not value then
            return nil
        end
        return tostring(value)
    end
end

local cx, cy
local function write_slk_line(x, y, k)
    if not k then
        return
    end
    local s = 'C'
    if x ~= cx then
        cx = x
        s = s .. ';X' .. x
    end
    if y ~= cy then
        cy = y
        s = s .. ';Y' .. y
    end
    if type(k) == 'string' then
        k = '"' .. k .. '"'
    elseif math.type(k) == 'float' then
        k = ('%.4f'):format(k):gsub('[0]+$', ''):gsub('%.$', '.0')
    end
    return s .. ';K' .. k
end

local function write_slk(type, slkname, t)
    local convert = {}
    if slk_keys[slkname] then
        for _, key in ipairs(slk_keys[slkname]) do
            convert[key:lower()] = key
        end
    end
    metadata = w2l:read_metadata(type)
    keydata1 = w2l:keyconvert(type)
    keydata2 = keydata1[slkname]
    firstkey = slk_keys[slkname][1]

    local rows = {}
    local cols = {}
    local colhash = {}
    for y, l in pairs(t) do
        cache[y] = l
        rows[#rows+1] = y
        local clean = {}
        for x, v in pairs(l) do
            colhash[x] = true
            if v == '_' then
                 if slkname == 'units\\unitabilities.slk' and key == 'auto'
                     or slkname == 'units\\unitbalance.slk' and (key == 'primary' or key == 'preventplace' or key == 'requireplace')
                     or slkname == 'units\\destructabledata.slk' and key == 'texfile'
                 then
                else
                    clean[x] = true
                end
            end
        end
        for x in pairs(clean) do
            l[x] = nil
        end
        if slkname == 'units\\abilitydata.slk' then
            for i = (l.levels or 0) + 1, 4 do
                for _, k in ipairs(abilkey) do
                    l[k:lower() .. i] = nil
                end
            end
        end
        l[slk_keys[slkname][1]] = y
    end
    if slkname == 'units\\unitbalance.slk' then
        colhash.lumberbountydice = true
        colhash.lumberbountyplus = true
        colhash.lumberbountysides = true
        colhash.repulseprio = true
    elseif slkname == 'units\\unitui.slk' then
        colhash.weap1 = nil
        colhash.weap2 = nil
        colhash.customteamcolor = true
        colhash.fograd = true
    elseif slkname == 'units\\unitdata.slk' then
        colhash.canbuildon = true
        colhash.requirewaterradius = true
    elseif slkname == 'units\\unitweapons.slk' then
        colhash.impactswimz = true
        colhash.launchswimz = true
    elseif slkname == 'units\\destructabledata.slk' then
        colhash.fatlos = true
        colhash.maxpitch = true
        colhash.maxroll = true
    elseif slkname == 'units\\upgradedata.slk' then
        colhash.used = nil
        colhash.code3 = true
        colhash.code4 = true
    elseif slkname == 'units\\itemdata.slk' then
        colhash.ignorecd = true
        colhash.morph = true
    elseif slkname == 'units\\abilitydata.slk' then
        colhash.datai1 = true
        colhash.datai2 = true
        colhash.datai3 = true
        colhash.datai4 = true
    end
    for x in pairs(colhash) do
        cols[#cols+1] = x
    end
    table.sort(rows, function (a, b)
        return a:lower() < b:lower()
    end)
    if slkname == 'units\\abilitydata.slk' then
        table.sort(cols, function (a, b)
            if a == 'alias' or b == 'alias' then
                return a == 'alias'
            end
            if a == 'code' or b == 'code' then
                return a == 'code'
            end
            return a < b
        end)
    elseif slkname == 'units\\unitui.slk' then
        table.sort(cols, function (a, b)
            if a == 'unituiid' or b == 'unituiid' then
                return a == 'unituiid'
            end
            if a == 'name' or b == 'name' then
                return a == 'name'
            end
            return a < b
        end)
    else
        table.sort(cols)
    end
    table.insert(cols, 1, slk_keys[slkname][1])
    cx = nil
    cy = nil
    local str = {}
    str[#str+1] = 'ID;PWXL;N;E'
    str[#str+1] = ('B;X%d;Y%d;D0'):format(#cols, #rows+1)
    for x, col in ipairs(cols) do
        str[#str+1] = write_slk_line(x, 1, convert[col] or col)
    end
    for y, row in ipairs(rows) do
        local l = t[row]
        for x, col in ipairs(cols) do
            local v = l[col]
            if v then
                str[#str+1] = write_slk_line(x, y + 1, to_type(get_key_type(col, l.code), v))
            elseif slkname == 'units\\unitabilities.slk' and col == 'auto'
                 or slkname == 'units\\unitbalance.slk' and (col == 'primary' or col == 'preventplace' or col == 'requireplace')
                 or slkname == 'units\\destructabledata.slk' and col == 'texfile'
             then
                str[#str+1] = write_slk_line(x, y + 1, '_')
            end
        end
    end
    str[#str+1] = 'E'
    return table.concat(str, '\r\n')
end

local function merge(o, name)
    if not o.levels then
        o[name] = {}
        return
    end
    for i = 1, o.levels do
        if o[name..i] then
            if not o[name] then
                o[name] = {}
            end
            o[name][i] = o[name..i]
            o[name..i] = nil
        end
    end
end

local function write_slk2(type, slkname, t)
    local keydata = w2l:keyconvert(type)
    for id, o in pairs(t) do
        o._id = id
        o._parent = id
        cache[id] = o
        if type == 'ability' then
            o._code = o.code
            if not keydata[o._parent] then
                o._parent = o.code
            end
            merge(o, 'Area')
            merge(o, 'BuffID')
            merge(o, 'Cast')
            merge(o, 'Cool')
            merge(o, 'Cost')
            merge(o, 'DataA')
            merge(o, 'DataB')
            merge(o, 'DataC')
            merge(o, 'DataD')
            merge(o, 'DataE')
            merge(o, 'DataF')
            merge(o, 'DataG')
            merge(o, 'DataH')
            merge(o, 'DataI')
            merge(o, 'Dur')
            merge(o, 'EfctID')
            merge(o, 'HeroDur')
            merge(o, 'Rng')
            merge(o, 'UnitID')
            merge(o, 'targs')
        elseif type == 'unit' then
            o._name = o.name
        end
    end
    return w2l:backend_slk(type, slkname, t)
end

w2l.info.slk.doodad = nil
for type, filelist in pairs(w2l.info.slk) do
    for _, filename in ipairs(filelist) do
        local inf = input / filename
        local outf = input / 'units2' / fs.path(filename):filename()
        local t = w2l:parse_slk(assert(io.load(inf)))
        if type == 'buff' then
            if t.XEsn then
                t.Xesn = t.XEsn
                t.XEsn = nil
            end
        end
        --io.save(outf, write_slk2(type, filename, t))
        io.save(outf, write_slk(type, filename, t))
    end
end

for type, filename in pairs(w2l.info.obj) do
    local inf = input / filename
    local outf = input / 'units2' / (fs.path(filename):filename():string() .. '.ini')
    local buf = io.load(inf)
    if buf then
        local data = w2l:frontend_obj(type, buf)
        for id, o in pairs(data) do
            if cache[id] then
                o._parent = cache[id]._parent
            else
                o._parent = id
            end
        end
        io.save(outf, w2l:backend_lni(type, data))
    end
end
