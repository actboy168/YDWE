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
        value = self.self:save_wts(self.wts, value, 'w3i里的文本长度超过1023字符')
    end
    return value
end

function mt:read_head(data)
    self:current '地图'
    data.file_ver   = self:get '文件版本'
    data.map_ver    = self:get '地图版本'
    data.editor_ver = self:get '编辑器版本'
    data.map_name   = self:get '地图名称'
    data.author     = self:get '作者名字'
    data.des        = self:get '地图描述'
    data.player_rec = self:get '推荐玩家'

    self:current '镜头'
    for i = 1, 8 do
        data['camera_bound_' .. i] = self:get '镜头范围' [i]
    end
    for i = 1, 4 do
        data['camera_complement_' .. i] = self:get '镜头范围扩充' [i]
    end

    self:current '地形'
    data.map_width = self:get '地图宽度'
    data.map_height = self:get '地图长度'
    data.map_main_ground_type = self:get '地形类型'

    self:current '选项'
    data.game_data_set = self:get '使用的游戏数据设置'
    data.map_flag = self:get '关闭预览图'      << 0
                  | self:get '自定义结盟优先权' << 1
                  | self:get '对战地图'        << 2
                  | self:get '大型地图'        << 3
                  | self:get '迷雾区域显示地形' << 4
                  | self:get '自定义玩家分组'   << 5
                  | self:get '自定义队伍'       << 6
                  | self:get '自定义科技树'     << 7
                  | self:get '自定义技能'       << 8
                  | self:get '自定义升级'       << 9
                  | self:get '地图菜单标记'     << 10
                  | self:get '地形悬崖显示水波' << 11
                  | self:get '地形起伏显示水波' << 12
                  | self:get '未知1'           << 13
                  | self:get '未知2'           << 14
                  | self:get '未知3'           << 15
                  | self:get '未知4'           << 16
                  | self:get '未知5'           << 17
                  | self:get '未知6'           << 18
                  | self:get '未知7'           << 19
                  | self:get '未知8'           << 20
                  | self:get '未知9'           << 21

    self:current '载入图'
    data.loading_screen_id       = self:get '序号'
    data.loading_screen_path     = self:get '路径'
    data.loading_screen_text     = self:get '文本'
    data.loading_screen_title    = self:get '标题'
    data.loading_screen_subtitle = self:get '子标题'

    self:current '战役'
    data.prologue_screen_path     = self:get '路径'
    data.prologue_screen_text     = self:get '文本'
    data.prologue_screen_title    = self:get '标题'
    data.prologue_screen_subtitle = self:get '子标题'

    self:current '迷雾'
    data.terrain_fog = self:get '类型'
    data.fog_start_z = self:get 'z轴起点'
    data.fog_end_z   = self:get 'z轴终点'
    data.fog_density = self:get '密度'
    data.fog_red     = self:get '颜色' [1]
    data.fog_green   = self:get '颜色' [2]
    data.fog_blue    = self:get '颜色' [3]
    data.fog_alpha   = self:get '颜色' [4]

    self:current '环境'
    data.weather_id        = self:get '天气'
    data.sound_environment = self:get '音效'
    data.light_environment = self:get '光照'
    data.water_red         = self:get '水面颜色' [1]
    data.water_green       = self:get '水面颜色' [2]
    data.water_blue        = self:get '水面颜色' [3]
    data.water_alpha       = self:get '水面颜色' [4]
end

function mt:read_player(data)
    self:current '玩家'
    data.player_count = self:get '玩家数量'
    data.players = {}

    for i = 1, data.player_count do
        local player = {}
        data.players[i] = player
        self:current('玩家' .. i)
        player.id             = self:get '玩家'
        player.type           = self:get '类型'
        player.race           = self:get '种族'
        player.start_position = self:get '修正出生点'
        player.name           = self:get '名字'
        player.start_x        = self:get '出生点' [1]
        player.start_y        = self:get '出生点' [2]
        player.ally_low_flag  = pack_flag(self:get '低结盟优先权标记')
        player.ally_high_flag = pack_flag(self:get '高结盟优先权标记')
    end
end

local function pack_unuse_players(self)
    local flag = 0xFFFFFFFF
    self:current '玩家'
    for i = 1, self:get '玩家数量' do
        self:current('玩家'..i)
        local id = self:get '玩家'
        flag = flag ~ (1 << id)
    end
    return flag
end

function mt:read_force(data)
    self:current '队伍'
    data.force_count = self:get '队伍数量'
    data.forces = {}

    local unuse_player_flag = pack_unuse_players(self)
    for i = 1, data.force_count do
        local force = {}
        data.forces[i] = force
        self:current('队伍' .. i)
        force.force_flag = self:get '结盟'            << 0
                         | self:get '结盟胜利'        << 1
                         | self:get '共享视野'        << 3
                         | self:get '共享单位控制'     << 4
                         | self:get '共享高级单位设置' << 5
        if i == 1 then
            force.player_flag = pack_flag(self:get '玩家列表') | unuse_player_flag
        else
            force.player_flag = pack_flag(self:get '玩家列表')
        end
        force.name        = self:get '队伍名称'
    end
end

function mt:read_upgrade(data)
    data.upgrades = {}
    local i = 1
    while self.data['升级'..i] do
        local upgrade = {}
        data.upgrades[i] = upgrade
        self:current('升级'..i)
        upgrade.player_flag = pack_flag(self:get '玩家列表')
        upgrade.id          = self:get 'ID'
        upgrade.level       = self:get '等级'
        upgrade.available   = self:get '可用性'
        i = i + 1
    end
    data.upgrade_count = #data.upgrades
end

function mt:read_tech(data)
    data.techs = {}
    local i = 1
    while self.data['科技'..i] do
        local tech = {}
        data.techs[i] = tech
        self:current('科技'..i)
        tech.player_flag = pack_flag(self:get '玩家列表')
        tech.id          = self:get 'ID'
        i = i + 1
    end
    data.tech_count = #data.techs
end

function mt:read_randomgroup(data)
    data.groups = {}
    local i = 1
    while self.data['随机组'..i] do
        local group = {}
        data.groups[i] = group
        self:current('随机组'..i)
        group.id = i - 1
        group.name = self:get '随机组名称'
        group.positions = self:get '位置类型'
        group.position_count = #group.positions
        group.line_count = #self:get '设置'
        group.lines = {}
        for i = 1, group.line_count do
            local line = {}
            group.lines[i] = line
            line.chance = self:get '设置' [i]['几率']
            line.ids    = self:get '设置' [i]['ID']
        end
        i = i + 1
    end
    data.group_count = #data.groups
end

function mt:read_randomitem(data)
    data.random_items = {}
    local i = 1
    while self.data['物品列表'..i] do
        local random_item = {}
        data.random_items[i] = random_item
        self:current('物品列表'..i)
        random_item.id = i - 1
        random_item.name = self:get '物品列表名称'
        random_item.sets = {}
        for i = 1, #self:get '设置' do
            local set = {}
            random_item.sets[i] = set
            set.items = {}
            for i, v in ipairs(self:get '设置' [i]) do
                local item = {}
                set.items[i] = item
                item.chance = v['几率']
                item.id     = v['ID']
            end
            set.item_count = #set.items
        end
        random_item.set_count = #random_item.sets
        i = i + 1
    end
    data.random_item_count = #data.random_items
end

function mt:add_head(data)
    self:add('lllzzzz', data.file_ver, data.map_ver, data.editor_ver, data.map_name, data.author, data.des, data.player_rec)

    self:add('ffffffff', data.camera_bound_1, data.camera_bound_2, data.camera_bound_3, data.camera_bound_4, data.camera_bound_5, data.camera_bound_6, data.camera_bound_7, data.camera_bound_8)

    self:add('llll', data.camera_complement_1, data.camera_complement_2, data.camera_complement_3, data.camera_complement_4)

    self:add('lllc1', data.map_width, data.map_height, data.map_flag, data.map_main_ground_type)

    self:add('lzzzz', data.loading_screen_id, data.loading_screen_path, data.loading_screen_text, data.loading_screen_title, data.loading_screen_subtitle)

    self:add('l', data.game_data_set)

    self:add('zzzz', data.prologue_screen_path, data.prologue_screen_text, data.prologue_screen_title, data.prologue_screen_subtitle)

    self:add('lfffBBBB', data.terrain_fog, data.fog_start_z, data.fog_end_z, data.fog_density, data.fog_red, data.fog_green, data.fog_blue, data.fog_alpha)

    self:add('c4zc1', data.weather_id, data.sound_environment, data.light_environment)

    self:add('BBBB', data.water_red, data.water_green, data.water_blue, data.water_alpha)
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

    tbl:read_head(data)
    tbl:read_player(data)
    tbl:read_force(data)
    tbl:read_upgrade(data)
    tbl:read_tech(data)
    tbl:read_randomgroup(data)
    tbl:read_randomitem(data)

    tbl:add_head(data)
    tbl:add_player(data)
    tbl:add_force(data)
    tbl:add_upgrade(data)
    tbl:add_tech(data)
    tbl:add_randomgroup(data)
    tbl:add_randomitem(data)

    return table_concat(tbl.hexs)
end
