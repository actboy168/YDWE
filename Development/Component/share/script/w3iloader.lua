return function (buf)
    local tbl = {}
    local pos = 1
    --文件头
    tbl.file_ver,                    --文件版本
    tbl.map_ver,                     --地图版本(保存次数)
    tbl.editor_ver,                  --编辑器版本
    tbl.map_name,                    --地图名称
    tbl.author,                      --作者名字
    tbl.des,                         --地图描述
    tbl.player_rec,                  --推荐玩家
    tbl.camera_bound_1,              --镜头范围
    tbl.camera_bound_2,
    tbl.camera_bound_3,
    tbl.camera_bound_4,
    tbl.camera_bound_5,
    tbl.camera_bound_6,
    tbl.camera_bound_7,
    tbl.camera_bound_8,
    tbl.camera_complement_1,         --镜头范围扩充
    tbl.camera_complement_2,
    tbl.camera_complement_3,
    tbl.camera_complement_4,
    tbl.map_width,                   --地图宽度
    tbl.map_height,                  --地图长度
    tbl.map_flag,                    --地图标记,后面解读
    tbl.map_main_ground_type,        --地形类型
    tbl.loading_screen_id,           --载入图ID(-1表示导入载入图)
    tbl.loading_screen_path,         --载入图路径
    tbl.loading_screen_text,         --载入界面文本
    tbl.loading_screen_title,        --载入界面标题
    tbl.loading_screen_subtitle,     --载入图子标题
    tbl.game_data_set,               --使用游戏数据设置
    tbl.prologue_screen_path,        --序幕路径
    tbl.prologue_screen_text,        --序幕文本
    tbl.prologue_screen_title,       --序幕标题
    tbl.prologue_screen_subtitle,    --序幕子标题
    tbl.terrain_fog,                 --地形迷雾
    tbl.fog_start_z,                 --迷雾开始z轴
    tbl.fog_end_z,                   --迷雾结束z轴
    tbl.fog_density,                 --迷雾密度
    tbl.fog_red,                     --迷雾红色
    tbl.fog_green,                   --迷雾绿色
    tbl.fog_blue,                    --迷雾蓝色
    tbl.fog_alpha,                   --迷雾透明
    tbl.weather_id,                  --全局天气
    tbl.sound_environment,           --环境音效
    tbl.light_environment,           --环境光照
    tbl.water_red,                   --水红色
    tbl.water_green,                 --水绿色
    tbl.water_blue,                  --水蓝色
    tbl.water_alpha,                 --水透明
    pos    = ('lllzzzzfffffffflllllllc1lzzzzlzzzzlfffBBBBc4zc1BBBB'):unpack(buf, pos)
    --玩家数据
    tbl.player_count, pos    = ('l'):unpack(buf, pos)
    tbl.players    = {}
    for i = 1, tbl.player_count do
        local player    = {}
        table.insert(tbl.players, player)
        player.id,
        player.type,            --玩家类型(1玩家,2电脑,3野怪,4可营救)
        player.race,            --玩家种族
        player.start_position,  --修正出生点
        player.name,
        player.start_x,
        player.start_y,
        player.ally_low_flag,   --低结盟优先权标记
        player.ally_high_flag,  --高结盟优先权标记
        pos    = ('llllzffll'):unpack(buf, pos)
    end
    --队伍数据
    tbl.force_count, pos    = ('l'):unpack(buf, pos)
    tbl.forces    = {}
    for i = 1, tbl.force_count do
        local force    = {}
        table.insert(tbl.forces, force)
        force.force_flag,     --队伍标记
        force.player_flag,    --包含玩家
        force.name,
        pos    = ('llz'):unpack(buf, pos)
    end
    --可用升级数据
    tbl.upgrade_count, pos    = ('l'):unpack(buf, pos)
    tbl.upgrades    = {}
    for i = 1, tbl.upgrade_count do
        local upgrade    = {}
        table.insert(tbl.upgrades, upgrade)

        upgrade.player_flag,       --包含玩家
        upgrade.id,                --4位ID
        upgrade.level,             --等级
        upgrade.available,         --可用性
        pos    = ('lc4ll'):unpack(buf, pos)
    end
    --可用科技数据
    tbl.tech_count, pos    = ('l'):unpack(buf, pos)
    tbl.techs    = {}
    for i = 1, tbl.tech_count do
        local tech    = {}
        table.insert(tbl.techs, tech)

        tech.player_flag,    --包含玩家
        tech.id,             --4位ID
        pos    = ('lc4'):unpack(buf, pos)
    end
    --随机组
    tbl.group_count, pos    = ('l'):unpack(buf, pos)
    tbl.groups    = {}
    for i = 1, tbl.group_count do
        local group    = {}
        table.insert(tbl.groups, group)
        group.id,
        group.name,
        pos    = ('lz'):unpack(buf, pos)
        --位置
        group.position_count,
        pos    = ('l'):unpack(buf, pos)
        group.positions    = {}
        for i = 1, group.position_count do
            group.positions[i], pos    = ('l'):unpack(buf, pos)
        end
        --设置
        group.line_count,
        pos    = ('l'):unpack(buf, pos)
        group.lines    = {}
        for i = 1, group.line_count do
            local line    = {}
            table.insert(group.lines, line)
            line.chance,
            pos    = ('l'):unpack(buf, pos)
            --id列举
            line.ids    = {}
            for i = 1, group.position_count do
                line.ids[i], pos    = ('c4'):unpack(buf, pos)
            end
        end
        
    end
    --物品列表
    tbl.random_item_count, pos    = ('l'):unpack(buf, pos)
    tbl.random_items    = {}
    for i = 1, tbl.random_item_count do
        local random_item    = {}
        table.insert(tbl.random_items, random_item)
        random_item.id,
        random_item.name,
        pos    = ('lz'):unpack(buf, pos)
        --设置
        random_item.set_count, pos    = ('l'):unpack(buf, pos)
        random_item.sets    = {}
        for i = 1, random_item.set_count do
            local set    = {}
            table.insert(random_item.sets, set)
            --物品
            set.item_count, pos    = ('l'):unpack(buf, pos)
            set.items    = {}
            for i = 1, set.item_count do
                local item    = {}
                table.insert(set.items, item)
                item.chance,
                item.id,
                pos    = ('lc4'):unpack(buf, pos)
            end
        end
    end
    return tbl
end
