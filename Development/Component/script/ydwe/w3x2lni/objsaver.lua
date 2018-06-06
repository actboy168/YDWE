local w3x2lni = require 'w3x2lni.init'
local stormlib = require 'ffi.stormlib'

local function get_map_flag(w3i)
    if not w3i then
        return 0
    end
    return w3i.CONFIG.DISABLE_PREVIEW          << 0
         | w3i.CONFIG.CUSTOM_ALLY              << 1
         | w3i.CONFIG.MELEE_MAP                << 2
         | w3i.CONFIG.LARGE_MAP                << 3
         | w3i.CONFIG.MASKED_AREA_SHOW_TERRAIN << 4
         | w3i.CONFIG.FIX_FORCE_SETTING        << 5
         | w3i.CONFIG.CUSTOM_FORCE             << 6
         | w3i.CONFIG.CUSTOM_TECHTREE          << 7
         | w3i.CONFIG.CUSTOM_ABILITY           << 8
         | w3i.CONFIG.CUSTOM_UPGRADE           << 9
         | w3i.CONFIG.MAP_MENU_MARK            << 10
         | w3i.CONFIG.SHOW_WAVE_ON_CLIFF       << 11
         | w3i.CONFIG.SHOW_WAVE_ON_ROLLING     << 12
         | w3i.CONFIG.UNKNOWN_1                << 13
         | w3i.CONFIG.UNKNOWN_2                << 14
         | w3i.CONFIG.UNKNOWN_3                << 15
         | w3i.CONFIG.UNKNOWN_4                << 16
         | w3i.CONFIG.UNKNOWN_5                << 17
         | w3i.CONFIG.UNKNOWN_6                << 18
         | w3i.CONFIG.UNKNOWN_7                << 19
         | w3i.CONFIG.UNKNOWN_8                << 20
         | w3i.CONFIG.UNKNOWN_9                << 21
end

local function get_player_count(w3i)
    local count = 0
    for i = 1, w3i.PLAYER.PLAYER_COUNT do
        local player = w3i['PLAYER'..i]
        if playerTYPE == 1 then
            count = count + 1
        end
    end
    return count
end

local function create_map(path, w3i, filecount, encrypt)
    local hexs = {}
    hexs[#hexs+1] = ('c4'):pack('HM3W')
    hexs[#hexs+1] = ('c4'):pack('\0\0\0\0')
    hexs[#hexs+1] = ('z'):pack(w3i and w3i.MAP.MAP_NAME or '未命名地图')
    hexs[#hexs+1] = ('l'):pack(get_map_flag(w3i))
    hexs[#hexs+1] = ('l'):pack(w3i and get_player_count(w3i) or 233)
    io.save(path, table.concat(hexs))
    log.info(('Create map at %s , size = %d'):format(path, filecount+3))
    local map = stormlib.create(path, filecount+3, encrypt)
    return map
end

return function (target_path, files)
    if not files['war3map.w3i'] then
        return false, 'No found: war3map.w3i'
    end
    local buf = io.load(files['war3map.w3i'])
    if not buf then
        return false, 'Read failed: war3map.w3i'
    end
    local w2l = w3x2lni()
    local w3i = w2l:frontend_w3i(buf)
    if not w3i then
        return false, 'Converted failed: war3map.w3i'
    end

    local max = 0
    for _ in pairs(files) do
        max = max + 1
    end

    local map = create_map(target_path, w3i, max, false)
    if not map then
        return false, 'Create map failed!'
    end
    for name, path in pairs(files) do
        local buf = io.load(path)
        if buf then
            log.info(('Imported: %s'):format(name))
            map:save_file(name, buf)
        else
            log.warn(('Read failed: %s'):format(name))
        end
    end
    map:close()
    log.info('Save map finish!')
    return true
end
