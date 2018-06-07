local w3x2lni = require 'w3x2lni.init'
local stormlib = require 'ffi.stormlib'

local w2l

local function get_map_flag(w3i)
    if not w3i then
        return 0
    end
    return w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.DISABLE_PREVIEW]          << 0
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.CUSTOM_ALLY]              << 1
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.MELEE_MAP]                << 2
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.LARGE_MAP]                << 3
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.MASKED_AREA_SHOW_TERRAIN] << 4
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.FIX_FORCE_SETTING]        << 5
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.CUSTOM_FORCE]             << 6
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.CUSTOM_TECHTREE]          << 7
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.CUSTOM_ABILITY]           << 8
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.CUSTOM_UPGRADE]           << 9
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.MAP_MENU_MARK]            << 10
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.SHOW_WAVE_ON_CLIFF]       << 11
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.SHOW_WAVE_ON_ROLLING]     << 12
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.UNKNOWN_1]                << 13
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.UNKNOWN_2]                << 14
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.UNKNOWN_3]                << 15
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.UNKNOWN_4]                << 16
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.UNKNOWN_5]                << 17
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.UNKNOWN_6]                << 18
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.UNKNOWN_7]                << 19
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.UNKNOWN_8]                << 20
         | w3i[w2l.lang.w3i.CONFIG][w2l.lang.w3i.UNKNOWN_9]                << 21
end

local function get_player_count(w3i)
    local count = 0
    for i = 1, w3i[w2l.lang.w3i.PLAYER][w2l.lang.w3i.PLAYER_COUNT] do
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
    hexs[#hexs+1] = ('z'):pack(w3i and w3i[w2l.lang.w3i.MAP][w2l.lang.w3i.MAP_NAME] or '未命名地图')
    hexs[#hexs+1] = ('l'):pack(get_map_flag(w3i))
    hexs[#hexs+1] = ('l'):pack(w3i and get_player_count(w3i) or 233)
    io.save(path, table.concat(hexs))
    log.info(('Create map at %s , size = %d'):format(path, filecount+3))
    local map = stormlib.create(path, filecount+3, encrypt)
    return map
end

local function scan(dir, callback, relative)
	if not relative then
		relative = fs.path ''
	end
	for path in dir:list_directory() do
		if fs.is_directory(path) then
			scan(path, callback, relative / path:filename())
		else
			callback(path, (relative / path:filename()):string())
		end
	end
end

return function (source_path, target_path)
    local files = {}
    scan(source_path, function (path, relative)
        files[relative] = path
    end)

    if not files['war3map.w3i'] then
        log.error('No found: war3map.w3i')
        return false
    end
    local buf = io.load(files['war3map.w3i'])
    if not buf then
        log.error('Read failed: war3map.w3i')
        return false
    end
    w2l = w3x2lni()
    local w3i = w2l:frontend_w3i(buf)
    if not w3i then
        log.error('Converted failed: war3map.w3i')
        return false
    end

    local max = 0
    for _ in pairs(files) do
        max = max + 1
    end

    local map = create_map(target_path, w3i, max, false)
    if not map then
        log.error('Create map failed!')
        return false
    end
    for name, path in pairs(files) do
        local buf = io.load(path)
        if buf then
            local suc = map:save_file(name, buf)
            if suc then
            else
                log.warn(('Imported failed: %s'):format(name))
            end
        else
            log.warn(('Read failed: %s'):format(name))
        end
    end
    map:close()
    log.info('Save map finish!')
    return true
end
