local lang = require 'lang'

local table_concat = table.concat
local type = type
local math_type = math.type

local mt = {}
mt.__index = mt

function mt:add(format, ...)
    self.lines[#self.lines+1] = format:format(...)
end

local function format_value(value)
    local tp = type(value)
    if tp == 'string' then
        if value:match '[\n\r]' then
            return ('[=[\r\n%s]=]'):format(value)
        else
            return ('%q'):format(value)
        end
    elseif tp == 'number' then
        if math_type(value) == 'integer' then
            return value
        else
            return ('%.4f'):format(value)
        end
    end
end

local function format_table(tbl)
    local new_tbl = {}
    for i = 1, #tbl do
        new_tbl[i] = format_value(tbl[i])
    end
    return new_tbl
end

function mt:title(name, data)
    if not data[name] then
        return nil
    end
    if self._title then
        self:add ''
    end
    self._title = data[name]
    self:add(('[%s]'):format(name))
    return data[name]
end

function mt:value(name)
    local value = self._title[name]
    if not value then
        return
    end
    if type(value) == 'table' then
        local strs = format_table(value)
        if type(strs[1]) == 'string' and #strs > 6 then
            self:add('%s = {\r\n%s\r\n}', name, table_concat(strs, ',\r\n'))
        else
            self:add('%s = {%s}', name, table_concat(strs, ', '))
        end
    else
        self:add('%s = %s', name, format_value(value))
    end
end

function mt:add_head(data)
    self:title(lang.w3i.MAP, data)
    self:value(lang.w3i.FILE_VERSION)
    self:value(lang.w3i.MAP_VERSION)
    self:value(lang.w3i.WE_VERSION)
    self:value(lang.w3i.MAP_NAME)
    self:value(lang.w3i.AUTHOR_NAME)
    self:value(lang.w3i.MAP_DESC)
    self:value(lang.w3i.PLAYER_DESC)

    self:title(lang.w3i.CAMERA, data)
    self:value(lang.w3i.CAMERA_BOUND)
    self:value(lang.w3i.CAMERA_COMPLEMENT)

    self:title(lang.w3i.MAP_INFO, data)
    self:value(lang.w3i.MAP_WIDTH)
    self:value(lang.w3i.MAP_HEIGHT)
    self:value(lang.w3i.MAP_MAIN_GROUND)
    
    self:title(lang.w3i.CONFIG, data)
    self:value(lang.w3i.GAME_DATA_SETTING)
    self:value(lang.w3i.DISABLE_PREVIEW)
    self:value(lang.w3i.CUSTOM_ALLY)
    self:value(lang.w3i.MELEE_MAP)
    self:value(lang.w3i.LARGE_MAP)
    self:value(lang.w3i.MASKED_AREA_SHOW_TERRAIN)
    self:value(lang.w3i.FIX_FORCE_SETTING)
    self:value(lang.w3i.CUSTOM_FORCE)
    self:value(lang.w3i.CUSTOM_TECHTREE)
    self:value(lang.w3i.CUSTOM_ABILITY)
    self:value(lang.w3i.CUSTOM_UPGRADE)
    self:value(lang.w3i.MAP_MENU_MARK)
    self:value(lang.w3i.SHOW_WAVE_ON_CLIFF)
    self:value(lang.w3i.SHOW_WAVE_ON_ROLLING)
    self:value(lang.w3i.UNKNOWN_1)
    self:value(lang.w3i.UNKNOWN_2)
    self:value(lang.w3i.UNKNOWN_3)
    self:value(lang.w3i.UNKNOWN_4)
    self:value(lang.w3i.UNKNOWN_5)
    self:value(lang.w3i.UNKNOWN_6)
    self:value(lang.w3i.UNKNOWN_7)
    self:value(lang.w3i.UNKNOWN_8)
    self:value(lang.w3i.UNKNOWN_9)
    
    self:title(lang.w3i.LOADING_SCREEN, data)
    self:value(lang.w3i.ID)
    self:value(lang.w3i.PATH)
    self:value(lang.w3i.TEXT)
    self:value(lang.w3i.TITLE)
    self:value(lang.w3i.SUBTITLE)

    self:title(lang.w3i.PROLOGUE, data)
    self:value(lang.w3i.ID)
    self:value(lang.w3i.PATH)
    self:value(lang.w3i.TEXT)
    self:value(lang.w3i.TITLE)
    self:value(lang.w3i.SUBTITLE)

    self:title(lang.w3i.FOG, data)
    self:value(lang.w3i.TYPE)
    self:value(lang.w3i.START_Z)
    self:value(lang.w3i.END_Z)
    self:value(lang.w3i.DENSITY)
    self:value(lang.w3i.COLOR)

    self:title(lang.w3i.ENVIRONMENT, data)
    self:value(lang.w3i.WEATHER)
    self:value(lang.w3i.SOUND)
    self:value(lang.w3i.LIGHT)
    self:value(lang.w3i.WATER_COLOR)

    return data
end

function mt:add_player(data)
    self:title(lang.w3i.PLAYER, data)
    self:value(lang.w3i.PLAYER_COUNT)

    for i = 1, data[lang.w3i.PLAYER][lang.w3i.PLAYER_COUNT] do
        self:title(lang.w3i.PLAYER..i, data)
        self:value(lang.w3i.PLAYER)
        self:value(lang.w3i.TYPE)
        self:value(lang.w3i.RACE)
        self:value(lang.w3i.FIX_START_POSITION)
        self:value(lang.w3i.NAME)
        self:value(lang.w3i.START_POSITION)
        self:value(lang.w3i.ALLY_LOW_FLAG)
        self:value(lang.w3i.ALLY_HIGH_FLAG)
    end
end

function mt:add_force(data)
    self:title(lang.w3i.FORCE, data)
    self:value(lang.w3i.FORCE_COUNT)

    for i = 1, data[lang.w3i.FORCE][lang.w3i.FORCE_COUNT] do
        self:title(lang.w3i.FORCE..i, data)
        self:value(lang.w3i.ALLY)
        self:value(lang.w3i.ALLY_WIN)
        self:value(lang.w3i.SHARE_VISIBLE)
        self:value(lang.w3i.SHARE_CONTROL)
        self:value(lang.w3i.SHARE_ADVANCE)
        self:value(lang.w3i.PLAYER_LIST)
        self:value(lang.w3i.FORCE_NAME)
    end
end

function mt:add_upgrade(data)
    local i = 1
    while self:title(lang.w3i.UPGRADE..i, data) do
        self:value(lang.w3i.PLAYER_LIST)
        self:value 'ID'
        self:value(lang.w3i.LEVEL)
        self:value(lang.w3i.AVAILABLE)
        i = i + 1
    end
end

function mt:add_tech(data)
    local i = 1
    while self:title(lang.w3i.TECH..i, data) do
        self:value(lang.w3i.PLAYER_LIST)
        self:value 'ID'
        i = i + 1
    end
end

function mt:add_randomgroup(data)
    local i = 1
    while self:title(lang.w3i.RANDOM_GROUP..i, data) do
        self:value(lang.w3i.RANDOM_GROUP_NAME)
        self:value(lang.w3i.POSITION_TYPE)

        self:add(lang.w3i.SETTING .. ' = {')
        for i, set in ipairs(data[lang.w3i.RANDOM_GROUP..i][lang.w3i.SETTING]) do
            self:add('%d = {', i)
            self:add(lang.w3i.CHANCE .. ' = %d,', set[lang.w3i.CHANCE])
            self:add('ID = {%s},', table_concat(format_table(set['ID']), ', '))
            self:add('},')
        end
        self:add('}')
        i = i + 1
    end
end

function mt:add_randomitem(data)
    local i = 1
    while self:title(lang.w3i.RANDOM_ITEM..i, data) do
        self:value(lang.w3i.RANDOM_ITEM_NAME)

        self:add(lang.w3i.SETTING .. ' = {')
        for i, set in ipairs(data[lang.w3i.RANDOM_ITEM..i][lang.w3i.SETTING]) do
            self:add('%d = {', i)
            for _, item in ipairs(set) do
                self:add(('{%s = %d, ID = %q},'):format(lang.w3i.CHANCE, item[lang.w3i.CHANCE], item['ID']))
            end
            self:add('},')
        end
        self:add('}')
        i = i + 1
    end
end

return function (w2l, data, wts)
    local tbl = setmetatable({}, mt)
    tbl.lines = {}
    tbl.self = w2l

    tbl:add_head(data)
    tbl:add_player(data)
    tbl:add_force(data)
    tbl:add_upgrade(data)
    tbl:add_tech(data)
    tbl:add_randomgroup(data)
    tbl:add_randomitem(data)

    return table_concat(tbl.lines, '\r\n')
end
