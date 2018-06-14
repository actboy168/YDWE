local stormlib = require 'ffi.stormlib'
local lang = require 'share.lang'

local function get_map_flag(w3i)
    if not w3i then
        return 0
    end
    return w3i[lang.w3i.CONFIG][lang.w3i.DISABLE_PREVIEW]          << 0
         | w3i[lang.w3i.CONFIG][lang.w3i.CUSTOM_ALLY]              << 1
         | w3i[lang.w3i.CONFIG][lang.w3i.MELEE_MAP]                << 2
         | w3i[lang.w3i.CONFIG][lang.w3i.LARGE_MAP]                << 3
         | w3i[lang.w3i.CONFIG][lang.w3i.MASKED_AREA_SHOW_TERRAIN] << 4
         | w3i[lang.w3i.CONFIG][lang.w3i.FIX_FORCE_SETTING]        << 5
         | w3i[lang.w3i.CONFIG][lang.w3i.CUSTOM_FORCE]             << 6
         | w3i[lang.w3i.CONFIG][lang.w3i.CUSTOM_TECHTREE]          << 7
         | w3i[lang.w3i.CONFIG][lang.w3i.CUSTOM_ABILITY]           << 8
         | w3i[lang.w3i.CONFIG][lang.w3i.CUSTOM_UPGRADE]           << 9
         | w3i[lang.w3i.CONFIG][lang.w3i.MAP_MENU_MARK]            << 10
         | w3i[lang.w3i.CONFIG][lang.w3i.SHOW_WAVE_ON_CLIFF]       << 11
         | w3i[lang.w3i.CONFIG][lang.w3i.SHOW_WAVE_ON_ROLLING]     << 12
         | w3i[lang.w3i.CONFIG][lang.w3i.UNKNOWN_1]                << 13
         | w3i[lang.w3i.CONFIG][lang.w3i.UNKNOWN_2]                << 14
         | w3i[lang.w3i.CONFIG][lang.w3i.UNKNOWN_3]                << 15
         | w3i[lang.w3i.CONFIG][lang.w3i.UNKNOWN_4]                << 16
         | w3i[lang.w3i.CONFIG][lang.w3i.UNKNOWN_5]                << 17
         | w3i[lang.w3i.CONFIG][lang.w3i.UNKNOWN_6]                << 18
         | w3i[lang.w3i.CONFIG][lang.w3i.UNKNOWN_7]                << 19
         | w3i[lang.w3i.CONFIG][lang.w3i.UNKNOWN_8]                << 20
         | w3i[lang.w3i.CONFIG][lang.w3i.UNKNOWN_9]                << 21
end

local function get_player_count(w3i)
    local count = 0
    for i = 1, w3i[lang.w3i.PLAYER][lang.w3i.PLAYER_COUNT] do
        local player = w3i[lang.w3i.PLAYER..i]
        if player[lang.w3i.TYPE] == 1 then
            count = count + 1
        end
    end
    return count
end

local mt = {}
mt.__index = mt

function mt:save(path, w3i, w3f, filecount, encrypt)
    if self.handle then
        self.handle:close()
        self.handle = nil
    end
    local hexs = {}
    if path:extension():string() == '.w3n' then
        hexs[#hexs+1] = ('c4'):pack('HM3W')
        hexs[#hexs+1] = ('c4'):pack('\0\0\0\0')
        hexs[#hexs+1] = ('z'):pack(w3f and w3f.campaign_name or '未命名战役')
        hexs[#hexs+1] = ('l'):pack(0)
        hexs[#hexs+1] = ('l'):pack(1)
    else
        hexs[#hexs+1] = ('c4'):pack('HM3W')
        hexs[#hexs+1] = ('c4'):pack('\0\0\0\0')
        hexs[#hexs+1] = ('z'):pack(w3i and w3i[lang.w3i.MAP][lang.w3i.MAP_NAME] or '未命名地图')
        hexs[#hexs+1] = ('l'):pack(get_map_flag(w3i))
        hexs[#hexs+1] = ('l'):pack(w3i and get_player_count(w3i) or 233)
    end
    io.save(path, table.concat(hexs))
    self.handle = stormlib.create(path, filecount+3, encrypt)
    if not self.handle then
        return false, lang.script.CREATE_MAP_FAILED
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
    if not self.handle then
        return nil
    end
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
    else
        handle = stormlib.open(input)
    end
    return setmetatable({ handle = handle, read = read }, mt)
end
