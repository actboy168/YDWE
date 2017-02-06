local stormlib = require 'ffi.stormlib'

local function get_map_flag(w3i)
    if not w3i then
        return 0
    end
    return w3i['选项']['关闭预览图']       << 0
         | w3i['选项']['自定义结盟优先权'] << 1
         | w3i['选项']['对战地图']        << 2
         | w3i['选项']['大型地图']        << 3
         | w3i['选项']['迷雾区域显示地形'] << 4
         | w3i['选项']['自定义玩家分组']   << 5
         | w3i['选项']['自定义队伍']       << 6
         | w3i['选项']['自定义科技树']     << 7
         | w3i['选项']['自定义技能']       << 8
         | w3i['选项']['自定义升级']       << 9
         | w3i['选项']['地图菜单标记']     << 10
         | w3i['选项']['地形悬崖显示水波'] << 11
         | w3i['选项']['地形起伏显示水波'] << 12
         | w3i['选项']['未知1']           << 13
         | w3i['选项']['未知2']           << 14
         | w3i['选项']['未知3']           << 15
         | w3i['选项']['未知4']           << 16
         | w3i['选项']['未知5']           << 17
         | w3i['选项']['未知6']           << 18
         | w3i['选项']['未知7']           << 19
         | w3i['选项']['未知8']           << 20
         | w3i['选项']['未知9']           << 21
end

local function get_player_count(w3i)
    local count = 0
    for i = 1, w3i['玩家']['玩家数量'] do
        local player = w3i['玩家'..i]
        if player['类型'] == 1 then
            count = count + 1
        end
    end
    return count
end

local mt = {}
mt.__index = mt

function mt:save(path, w3i, n, encrypt)
    if self.handle then
        self.handle:close()
        self.handle = nil
    end
    local hexs = {}
    hexs[#hexs+1] = ('c4'):pack('HM3W')
    hexs[#hexs+1] = ('c4'):pack('\0\0\0\0')
    hexs[#hexs+1] = ('z'):pack(w3i and w3i['地图']['地图名称'] or '未命名地图')
    hexs[#hexs+1] = ('l'):pack(get_map_flag(w3i))
    hexs[#hexs+1] = ('l'):pack(w3i and get_player_count(w3i) or 233)
    io.save(path, table.concat(hexs))
    self.handle = stormlib.create(path, n+3, encrypt)
    if not self.handle then
        return false
    end
    return true
end

function mt:close()
    return self.handle:close()
end

function mt:extract(name, path)
    return self.handle:extract(name, path)
end

function mt:has_file(name)
    if not self.read then
        return
    end
    return self.handle:has_file(name)
end

function mt:remove_file(name)
    if self.read then
        return false
    end
    return self.handle:remove_file(name)
end

function mt:load_file(name)
    return self.handle:load_file(name)
end

function mt:save_file(name, buf, filetime)
    if self.read then
        return false
    end
    return self.handle:save_file(name, buf, filetime)
end

function mt:number_of_files()
    return self.handle:number_of_files()
end

return function (input, read)
    local handle
    if type(input) == 'number' then
        handle = stormlib.attach(input)
    elseif read then
        handle = stormlib.open(input, true)
        if not handle then
            return nil
        end
        if not handle:has_file '(listfile)' then
            message('不支持没有(listfile)的地图')
            return nil
        end
    else
        handle = stormlib.open(input)
    end
    return setmetatable({ handle = handle, read = read }, mt)
end
