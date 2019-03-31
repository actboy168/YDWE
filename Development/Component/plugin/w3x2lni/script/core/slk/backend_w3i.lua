local lang = require 'lang'
local table_insert = table.insert
local table_sort   = table.sort
local table_concat = table.concat
local string_char  = string.char
local type = type
local pairs = pairs
local setmetatable = setmetatable

local function pack_flag(data)
    local flag = 0
    for i = 1, #data do
        flag = flag + (1 << (data[i]-1))
    end
    return flag
end

local mt = {}
mt.__index = mt

function mt:add(format, ...)
    self.hexs[#self.hexs+1] = (format):pack(...)
end

function mt:current(key)
    self._current = key
end

function mt:get(key)
    local value = self.data[self._current][key]
    if type(value) == 'string' and #value > 1023 then
        value = self.self:save_wts(self.wts, value, lang.script.TEXT_TOO_LONG_IN_W3I)
    end
    return value
end

function mt:read_head(data, version)
    self:current(lang.w3i.MAP)
    data.file_ver   = version
    data.map_ver    = self:get(lang.w3i.MAP_VERSION)
    data.editor_ver = self:get(lang.w3i.WE_VERSION)
    data.map_name   = self:get(lang.w3i.MAP_NAME)
    data.author     = self:get(lang.w3i.AUTHOR_NAME)
    data.des        = self:get(lang.w3i.MAP_DESC)
    data.player_rec = self:get(lang.w3i.PLAYER_DESC)

    self:current(lang.w3i.CAMERA)
    for i = 1, 8 do
        data['camera_bound_' .. i] = self:get(lang.w3i.CAMERA_BOUND)[i]
    end
    for i = 1, 4 do
        data['camera_complement_' .. i] = self:get(lang.w3i.CAMERA_COMPLEMENT)[i]
    end

    self:current(lang.w3i.MAP_INFO)
    data.map_width = self:get(lang.w3i.MAP_WIDTH)
    data.map_height = self:get(lang.w3i.MAP_HEIGHT)
    data.map_main_ground_type = self:get(lang.w3i.MAP_MAIN_GROUND)

    self:current(lang.w3i.CONFIG)
    data.game_data_set = self:get(lang.w3i.GAME_DATA_SETTING)
    data.map_flag = self:get(lang.w3i.DISABLE_PREVIEW)          << 0
                  | self:get(lang.w3i.CUSTOM_ALLY)              << 1
                  | self:get(lang.w3i.MELEE_MAP)                << 2
                  | self:get(lang.w3i.LARGE_MAP)                << 3
                  | self:get(lang.w3i.MASKED_AREA_SHOW_TERRAIN) << 4
                  | self:get(lang.w3i.FIX_FORCE_SETTING)        << 5
                  | self:get(lang.w3i.CUSTOM_FORCE)             << 6
                  | self:get(lang.w3i.CUSTOM_TECHTREE)          << 7
                  | self:get(lang.w3i.CUSTOM_ABILITY)           << 8
                  | self:get(lang.w3i.CUSTOM_UPGRADE)           << 9
                  | self:get(lang.w3i.MAP_MENU_MARK)            << 10
                  | self:get(lang.w3i.SHOW_WAVE_ON_CLIFF)       << 11
                  | self:get(lang.w3i.SHOW_WAVE_ON_ROLLING)     << 12
                  | self:get(lang.w3i.UNKNOWN_1)                 << 13
                  | self:get(lang.w3i.UNKNOWN_2)                 << 14
                  | self:get(lang.w3i.UNKNOWN_3)                 << 15
                  | self:get(lang.w3i.UNKNOWN_4)                 << 16
                  | self:get(lang.w3i.UNKNOWN_5)                 << 17
                  | self:get(lang.w3i.UNKNOWN_6)                 << 18
                  | self:get(lang.w3i.UNKNOWN_7)                 << 19
                  | self:get(lang.w3i.UNKNOWN_8)                 << 20
                  | self:get(lang.w3i.UNKNOWN_9)                 << 21

    self:current(lang.w3i.LOADING_SCREEN)
    data.loading_screen_id       = self:get(lang.w3i.ID)
    data.loading_screen_path     = self:get(lang.w3i.PATH)
    data.loading_screen_text     = self:get(lang.w3i.TEXT)
    data.loading_screen_title    = self:get(lang.w3i.TITLE)
    data.loading_screen_subtitle = self:get(lang.w3i.SUBTITLE)

    self:current(lang.w3i.PROLOGUE)
    data.prologue_screen_id     = self:get(lang.w3i.ID)
    data.prologue_screen_path     = self:get(lang.w3i.PATH)
    data.prologue_screen_text     = self:get(lang.w3i.TEXT)
    data.prologue_screen_title    = self:get(lang.w3i.TITLE)
    data.prologue_screen_subtitle = self:get(lang.w3i.SUBTITLE)

    if version == 25 then
        self:current(lang.w3i.FOG)
        data.terrain_fog = self:get(lang.w3i.TYPE)
        data.fog_start_z = self:get(lang.w3i.START_Z)
        data.fog_end_z   = self:get(lang.w3i.END_Z)
        data.fog_density = self:get(lang.w3i.DENSITY)
        data.fog_red     = self:get(lang.w3i.COLOR)[1]
        data.fog_green   = self:get(lang.w3i.COLOR)[2]
        data.fog_blue    = self:get(lang.w3i.COLOR)[3]
        data.fog_alpha   = self:get(lang.w3i.COLOR)[4]

        self:current(lang.w3i.ENVIRONMENT)
        data.weather_id        = self:get(lang.w3i.WEATHER)
        data.sound_environment = self:get(lang.w3i.SOUND)
        data.light_environment = self:get(lang.w3i.LIGHT)
        data.water_red         = self:get(lang.w3i.WATER_COLOR)[1]
        data.water_green       = self:get(lang.w3i.WATER_COLOR)[2]
        data.water_blue        = self:get(lang.w3i.WATER_COLOR)[3]
        data.water_alpha       = self:get(lang.w3i.WATER_COLOR)[4]
    end
end

function mt:read_player(data)
    self:current(lang.w3i.PLAYER)
    data.player_count = self:get(lang.w3i.PLAYER_COUNT)
    data.players = {}

    for i = 1, data.player_count do
        local player = {}
        data.players[i] = player
        self:current(lang.w3i.PLAYER .. i)
        player.id             = self:get(lang.w3i.PLAYER)
        player.type           = self:get(lang.w3i.TYPE)
        player.race           = self:get(lang.w3i.RACE)
        player.start_position = self:get(lang.w3i.FIX_START_POSITION)
        player.name           = self:get(lang.w3i.NAME)
        player.start_x        = self:get(lang.w3i.START_POSITION)[1]
        player.start_y        = self:get(lang.w3i.START_POSITION)[2]
        player.ally_low_flag  = pack_flag(self:get(lang.w3i.ALLY_LOW_FLAG))
        player.ally_high_flag = pack_flag(self:get(lang.w3i.ALLY_HIGH_FLAG))
    end
end

local function pack_unuse_players(self)
    local flag = 0xFFFFFFFF
    self:current(lang.w3i.PLAYER)
    for i = 1, self:get(lang.w3i.PLAYER_COUNT) do
        self:current(lang.w3i.PLAYER .. i)
        local id = self:get(lang.w3i.PLAYER)
        flag = flag ~ (1 << id)
    end
    return flag
end

function mt:read_force(data)
    self:current(lang.w3i.FORCE)
    data.force_count = self:get(lang.w3i.FORCE_COUNT)
    data.forces = {}

    local unuse_player_flag = pack_unuse_players(self)
    for i = 1, data.force_count do
        local force = {}
        data.forces[i] = force
        self:current(lang.w3i.FORCE .. i)
        force.force_flag = self:get(lang.w3i.ALLY)            << 0
                         | self:get(lang.w3i.ALLY_WIN)        << 1
                         | self:get(lang.w3i.SHARE_VISIBLE)        << 3
                         | self:get(lang.w3i.SHARE_CONTROL)     << 4
                         | self:get(lang.w3i.SHARE_ADVANCE) << 5
        if i == 1 then
            force.player_flag = pack_flag(self:get(lang.w3i.PLAYER_LIST)) | unuse_player_flag
        else
            force.player_flag = pack_flag(self:get(lang.w3i.PLAYER_LIST))
        end
        force.name        = self:get(lang.w3i.FORCE_NAME)
    end
end

function mt:read_upgrade(data)
    data.upgrades = {}
    local i = 1
    while self.data[lang.w3i.UPGRADE..i] do
        local upgrade = {}
        data.upgrades[i] = upgrade
        self:current(lang.w3i.UPGRADE..i)
        upgrade.player_flag = pack_flag(self:get(lang.w3i.PLAYER_LIST))
        upgrade.id          = self:get 'ID'
        upgrade.level       = self:get(lang.w3i.LEVEL)
        upgrade.available   = self:get(lang.w3i.AVAILABLE)
        i = i + 1
    end
    data.upgrade_count = #data.upgrades
end

function mt:read_tech(data)
    data.techs = {}
    local i = 1
    while self.data[lang.w3i.TECH..i] do
        local tech = {}
        data.techs[i] = tech
        self:current(lang.w3i.TECH..i)
        tech.player_flag = pack_flag(self:get(lang.w3i.PLAYER_LIST))
        tech.id          = self:get 'ID'
        i = i + 1
    end
    data.tech_count = #data.techs
end

function mt:read_randomgroup(data)
    data.groups = {}
    local i = 1
    while self.data[lang.w3i.RANDOM_GROUP..i] do
        local group = {}
        data.groups[i] = group
        self:current(lang.w3i.RANDOM_GROUP..i)
        group.id = i - 1
        group.name = self:get(lang.w3i.RANDOM_GROUP_NAME)
        group.positions = self:get(lang.w3i.POSITION_TYPE)
        group.position_count = #group.positions
        group.line_count = #self:get(lang.w3i.SETTING)
        group.lines = {}
        for i = 1, group.line_count do
            local line = {}
            group.lines[i] = line
            line.chance = self:get(lang.w3i.SETTING)[i][lang.w3i.CHANCE]
            line.ids    = self:get(lang.w3i.SETTING)[i]['ID']
        end
        i = i + 1
    end
    data.group_count = #data.groups
end

function mt:read_randomitem(data)
    data.random_items = {}
    local i = 1
    while self.data[lang.w3i.RANDOM_ITEM..i] do
        local random_item = {}
        data.random_items[i] = random_item
        self:current(lang.w3i.RANDOM_ITEM..i)
        random_item.id = i - 1
        random_item.name = self:get(lang.w3i.RANDOM_ITEM_NAME)
        random_item.sets = {}
        for i = 1, #self:get(lang.w3i.SETTING) do
            local set = {}
            random_item.sets[i] = set
            set.items = {}
            for i, v in ipairs(self:get(lang.w3i.SETTING)[i]) do
                local item = {}
                set.items[i] = item
                item.chance = v[lang.w3i.CHANCE]
                item.id     = v['ID']
            end
            set.item_count = #set.items
        end
        random_item.set_count = #random_item.sets
        i = i + 1
    end
    data.random_item_count = #data.random_items
end

function mt:add_head(data, version)
    self:add('lllzzzz', version, data.map_ver, data.editor_ver, data.map_name, data.author, data.des, data.player_rec)

    self:add('ffffffff', data.camera_bound_1, data.camera_bound_2, data.camera_bound_3, data.camera_bound_4, data.camera_bound_5, data.camera_bound_6, data.camera_bound_7, data.camera_bound_8)

    self:add('llll', data.camera_complement_1, data.camera_complement_2, data.camera_complement_3, data.camera_complement_4)

    self:add('lllc1', data.map_width, data.map_height, data.map_flag, data.map_main_ground_type)

    if version == 25 then
        self:add('lzzzz', data.loading_screen_id, data.loading_screen_path, data.loading_screen_text, data.loading_screen_title, data.loading_screen_subtitle)

        self:add('l', data.game_data_set)

        self:add('zzzz', data.prologue_screen_path, data.prologue_screen_text, data.prologue_screen_title, data.prologue_screen_subtitle)

        self:add('lfffBBBB', data.terrain_fog, data.fog_start_z, data.fog_end_z, data.fog_density, data.fog_red, data.fog_green, data.fog_blue, data.fog_alpha)

        self:add('c4zc1', data.weather_id, data.sound_environment, data.light_environment)

        self:add('BBBB', data.water_red, data.water_green, data.water_blue, data.water_alpha)
    elseif version == 18 then
        self:add('lzzz', data.loading_screen_id, data.loading_screen_text, data.loading_screen_title, data.loading_screen_subtitle)

        self:add('lzzz', data.prologue_screen_id, data.prologue_screen_text, data.prologue_screen_title, data.prologue_screen_subtitle)
    end
end

function mt:add_player(data)
    self:add('l', data.player_count)

    for i = 1, data.player_count do
        local player = data.players[i]

        --player.ally_low_flag = player.ally_low_flag | ((1 << data.player_count) - 1)
        --player.ally_high_flag = player.ally_high_flag | ((1 << data.player_count) - 1)

        self:add('llllzffLL', player.id, player.type, player.race, player.start_position, player.name, player.start_x, player.start_y, player.ally_low_flag, player.ally_high_flag)
    end
end

function mt:add_force(data)
    self:add('l', data.force_count)

    for i = 1, data.force_count do
        local force = data.forces[i]

        --force.player_flag = force.player_flag | ((1 << data.player_count) - 1)

        self:add('LLz', force.force_flag, force.player_flag, force.name)
    end
end

function mt:add_upgrade(data)
    self:add('l', data.upgrade_count)

    for i = 1, data.upgrade_count do
        local upgrade = data.upgrades[i]

        self:add('lc4ll', upgrade.player_flag, upgrade.id, upgrade.level, upgrade.available)
    end
end

function mt:add_tech(data)
    self:add('l', data.tech_count)
    
    for i = 1, data.tech_count do
        local tech = data.techs[i]

        self:add('lc4', tech.player_flag, tech.id)
    end
end

function mt:add_randomgroup(data)
    self:add('l', data.group_count)

    for i = 1, data.group_count do
        local group = data.groups[i]

        self:add('lz', group.id, group.name)

        self:add('l', group.position_count)
        
        for i = 1, group.position_count do
            self:add('l', group.positions[i])
        end

        self:add('l', group.line_count)

        for i = 1, group.line_count do
            local line = group.lines[i]

            self:add('l', line.chance)

            for i = 1, group.position_count do
                self:add('c4', line.ids[i])
            end
        end
    end
end

function mt:add_randomitem(data)
    self:add('l', data.random_item_count)

    for i = 1, data.random_item_count do
        local random_item = data.random_items[i]

        self:add('lz', random_item.id, random_item.name)

        self:add('l', random_item.set_count)

        for i = 1, random_item.set_count do
            local set = random_item.sets[i]

            self:add('l', set.item_count)

            for i = 1, set.item_count do
                local item = set.items[i]

                self:add('lc4', item.chance, item.id)
            end
        end
    end
end

return function (self, data, wts)
    local tbl = setmetatable({}, mt)
    tbl.hexs = {}
    tbl.data = data
    tbl.self = self
    tbl.wts  = wts

    local data = {}
    local version = tbl.data[lang.w3i.MAP][lang.w3i.FILE_VERSION]
    if version == 25 then
        tbl:read_head(data, version)
        tbl:read_player(data)
        tbl:read_force(data)
        tbl:read_upgrade(data)
        tbl:read_tech(data)
        tbl:read_randomgroup(data)
        tbl:read_randomitem(data)
    
        tbl:add_head(data, version)
        tbl:add_player(data)
        tbl:add_force(data)
        tbl:add_upgrade(data)
        tbl:add_tech(data)
        tbl:add_randomgroup(data)
        tbl:add_randomitem(data)
    elseif version == 18 then
        tbl:read_head(data, version)
        tbl:read_player(data)
        tbl:read_force(data)
        tbl:read_upgrade(data)
        tbl:read_tech(data)
        tbl:read_randomgroup(data)
    
        tbl:add_head(data, version)
        tbl:add_player(data)
        tbl:add_force(data)
        tbl:add_upgrade(data)
        tbl:add_tech(data)
        tbl:add_randomgroup(data)
    end

    return table_concat(tbl.hexs)
end
