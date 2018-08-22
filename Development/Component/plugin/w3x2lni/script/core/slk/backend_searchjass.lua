local lpeg = require 'lpeglabel'
local w3xparser = require 'w3xparser'
local lang = require 'lang'
local wtonumber = w3xparser.tonumber
local ids
local marks
local line_count
local min = ('>I4'):unpack 'A000'

local extra_func = {
    MeleeStartingUnitsHuman = {'hpea', 'Hamg', 'Hpal', 'Hblm', 'Hmkg', 'htow', 'Amic', 'stwp'},
    MeleeStartingUnitsOrc = {'opeo', 'Obla', 'Ofar', 'ogre', 'Otch', 'Oshd', 'stwp'},
    MeleeStartingUnitsUndead = {'Udre', 'Udea', 'Ucrl', 'uaco', 'unpl', 'ugho', 'Ulic', 'stwp'},
    MeleeStartingUnitsNightElf = {'etol', 'Edem', 'ewsp', 'Ewar', 'Emoo', 'Ekee', 'stwp'},
    MeleeStartingUnitsUnknownRace = {'nshe'},
    MeleeStartingUnitsForPlayer = {'hpea', 'Hamg', 'Hpal', 'Hblm', 'Hmkg', 'htow', 'Amic', 'opeo', 'Obla', 'Ofar', 'ogre', 'Otch', 'Oshd', 'Udre', 'Udea', 'Ucrl', 'uaco', 'unpl', 'ugho', 'Ulic', 'etol', 'Edem', 'ewsp', 'Ewar', 'Emoo', 'Ekee', 'stwp'},
    MeleeStartingUnits = {'hpea', 'Hamg', 'Hpal', 'Hblm', 'Hmkg', 'htow', 'Amic', 'opeo', 'Obla', 'Ofar', 'ogre', 'Otch', 'Oshd', 'Udre', 'Udea', 'Ucrl', 'uaco', 'unpl', 'ugho', 'Ulic', 'etol', 'Edem', 'ewsp', 'Ewar', 'Emoo', 'Ekee', 'nshe', 'stwp'},
    
    MeleeGrantItemsToHero = {'stwp'},
    MeleeGrantItemsToTrainedHero = {'stwp'},
    MeleeGrantItemsToHiredHero = {'stwp'},
    MeleeGrantHeroItems = {'stwp'},
    MeleeRandomHeroLoc = {'stwp'},

    ChangeElevatorWallBlocker = {'DTep'},
    NearbyElevatorExistsEnum = {'DTrx', 'DTrf'},
    NearbyElevatorExists = {'DTrx', 'DTrf'},
    ChangeElevatorWalls = {'DTep', 'DTrx', 'DTrf'},
}

local need_mark = {
    ChooseRandomCreep = 'creeps',
    ChooseRandomCreepBJ = 'creeps',
    ChooseRandomNPBuilding = 'building',
    ChooseRandomNPBuildingBJ = 'building',
    ChooseRandomItem = 'item',
    ChooseRandomItemBJ = 'item',
    ChooseRandomItemEx = 'item',
    ChooseRandomItemExBJ = 'item',
    UpdateEachStockBuildingEnum = 'item',
    UpdateEachStockBuilding = 'marketplace',
    PerformStockUpdates = 'marketplace',
    StartStockUpdates = 'marketplace',
    InitNeutralBuildings = 'marketplace',
    InitBlizzard = 'marketplace',
}

local function add_id(id)
    if #id ~= 4 then
        return
    end
    ids[id] = true
end

local function fint1(str)
    local int = wtonumber(str)
    if int < min then
        return
    end
    add_id(('>I4'):pack(int))
end

local function fint3(str)
    fint1('0x'..str:sub(2))
end

local function fint4(str)
    add_id(str:sub(2, -2))
end

local function fbj(id)
    if extra_func[id] then
        for _, name in ipairs(extra_func[id]) do
            ids[name] = true
        end
    end
    if need_mark[id] then
        if need_mark[id] == 'creeps' and not marks.creeps then
            w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, lang.report.RETAIN_CREEP_UNIT, (lang.report.REFERENCE_BY_JASS):format(id))
        end
        if need_mark[id] == 'building' and not marks.building then
            w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, lang.report.RETAIN_CREEP_BUILDING, (lang.report.REFERENCE_BY_JASS):format(id))
        end
        if need_mark[id] == 'item' and not marks.item then
            w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, lang.report.RETAIN_RANDOM_ITEM, (lang.report.REFERENCE_BY_JASS):format(id))
        end
        marks[need_mark[id]] = true
    end
end

lpeg.locale(lpeg)
local S = lpeg.S
local P = lpeg.P
local R = lpeg.R

local nl1  = P'\r\n' + S'\r\n'
local com  = P'//' * (1-nl1)^0
local sp   = (S' \t' + P'\xEF\xBB\xBF' + com)^0
local sps  = (S' \t' + P'\xEF\xBB\xBF' + com)^1
local nl   = com^0 * nl1 / function() line_count = line_count + 1 end
local ign  = sps + nl
local quo  = P'"'
local iquo = P"'"
local esc  = P'\\'
local int1 = (P'-' * sp)^-1 * (P'0' + R'19' * R'09'^0) / fint1
local int2 = P'0' * S'xX' * R('af', 'AF', '09')^1 / fint1
local int3 = P'$' * R('af', 'AF', '09')^1 / fint3
local int_ = esc * P(1) + (1-iquo)
local int4 = iquo * int_^1^-4 * iquo / fint4
local int  = int4 + int3 + int2 + int1
local real = (P'-' * sp)^-1 * (P'.' * R'09'^1 + R'09'^1 * P'.' * R'09'^0)
local str1 = esc * P(1) + (1-quo)
local str  = quo * (nl1 + str1)^0 * quo
local id   = R('az', 'AZ') * R('az', 'AZ', '09', '__')^0 / fbj

local function err(str)
    return ((1-nl)^1 + P(1)) / function(c) error(('\n\n%s\n\n第 %d 行：\n===========================\n%s\n===========================\n'):format(str, line_count, c)) end
end

local word = sp * (real + int + str + id) * sp
local pjass = (ign + word + S'=+-*/><!()[],' + err(lang.report.SYNTAX_ERROR))^0

return function (w2l_)
    w2l = w2l_
    local buf = w2l:file_load('map', 'war3map.j')
    if not buf then
        buf = w2l:file_load('map', 'scripts\\war3map.j')
        if not buf then
            return
        end
    end
    ids = {}
    marks = {}
    line_count = 0
    local suc, err = pcall(pjass.match, pjass, buf)
    if not suc then
        w2l.messager.report(lang.report.ERROR, 1, lang.report.SYNTAX_ERROR, err:match('%.lua:%d+: (.*)'))
        return
    end
    return ids, marks
end
