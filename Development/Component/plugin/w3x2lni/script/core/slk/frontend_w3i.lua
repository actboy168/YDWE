local lang = require 'lang'
local select = select

local w2l

local function unpack_flag(flag)
    local tbl = {}
    for i = 0, 64 do
        local n = 1 << i
        if n > flag then
            break
        end
        if flag & n ~= 0 then
            tbl[#tbl+1] = i + 1
        end
    end
    return tbl
end

local function pack(...)
    local tbl = {...}
    tbl[#tbl] = nil
    return tbl
end

local mt = {}
mt.__index = mt

function mt:set_index(...)
    self.index = select(-1, ...)
    return ...
end

function mt:unpack(str)
    return self:set_index(str:unpack(self.content, self.index))
end

function mt:is_finish()
    return ('I1'):unpack(self.content, self.index) == 0xFF
end

function mt:get_version(chunk)
    local version = self:unpack 'l'
    return version
end

function mt:add_head(chunk, version)
    chunk[lang.w3i.MAP] = {
        [lang.w3i.FILE_VERSION] = version,
        [lang.w3i.MAP_VERSION]  = self:unpack 'l',
        [lang.w3i.WE_VERSION]   = self:unpack 'l',
        [lang.w3i.MAP_NAME]     = w2l:load_wts(self.wts, (self:unpack 'z')),
        [lang.w3i.AUTHOR_NAME]  = w2l:load_wts(self.wts, (self:unpack 'z')),
        [lang.w3i.MAP_DESC]     = w2l:load_wts(self.wts, (self:unpack 'z')),
        [lang.w3i.PLAYER_DESC]  = w2l:load_wts(self.wts, (self:unpack 'z')),
    }
    
    chunk[lang.w3i.CAMERA] = {
        [lang.w3i.CAMERA_BOUND]      = pack(self:unpack 'ffffffff'),
        [lang.w3i.CAMERA_COMPLEMENT] = pack(self:unpack 'llll'),
    }

    chunk[lang.w3i.MAP_INFO] = {
        [lang.w3i.MAP_WIDTH]  = self:unpack 'l',
        [lang.w3i.MAP_HEIGHT] = self:unpack 'l',
    }

    local flag = self:unpack 'L'
    chunk[lang.w3i.CONFIG] = {
        [lang.w3i.DISABLE_PREVIEW]          = flag >> 0 & 1,
        [lang.w3i.CUSTOM_ALLY]              = flag >> 1 & 1,
        [lang.w3i.MELEE_MAP]                = flag >> 2 & 1,
        [lang.w3i.LARGE_MAP]                = flag >> 3 & 1,
        [lang.w3i.MASKED_AREA_SHOW_TERRAIN] = flag >> 4 & 1,
        [lang.w3i.FIX_FORCE_SETTING]        = flag >> 5 & 1,
        [lang.w3i.CUSTOM_FORCE]             = flag >> 6 & 1,
        [lang.w3i.CUSTOM_TECHTREE]          = flag >> 7 & 1,
        [lang.w3i.CUSTOM_ABILITY]           = flag >> 8 & 1,
        [lang.w3i.CUSTOM_UPGRADE]           = flag >> 9 & 1,
        [lang.w3i.MAP_MENU_MARK]            = flag >> 10 & 1,
        [lang.w3i.SHOW_WAVE_ON_CLIFF]       = flag >> 11 & 1,
        [lang.w3i.SHOW_WAVE_ON_ROLLING]     = flag >> 12 & 1,
        [lang.w3i.UNKNOWN_1]                = flag >> 13 & 1,
        [lang.w3i.UNKNOWN_2]                = flag >> 14 & 1,
        [lang.w3i.UNKNOWN_3]                = flag >> 15 & 1,
        [lang.w3i.UNKNOWN_4]                = flag >> 16 & 1,
        [lang.w3i.UNKNOWN_5]                = flag >> 17 & 1,
        [lang.w3i.UNKNOWN_6]                = flag >> 18 & 1,
        [lang.w3i.UNKNOWN_7]                = flag >> 19 & 1,
        [lang.w3i.UNKNOWN_8]                = flag >> 20 & 1,
        [lang.w3i.UNKNOWN_9]                = flag >> 21 & 1,
    }

    chunk[lang.w3i.MAP_INFO][lang.w3i.MAP_MAIN_GROUND] = self:unpack 'c1'

    if version == 25 then
        chunk[lang.w3i.LOADING_SCREEN] = {
            [lang.w3i.ID]       = self:unpack 'l',
            [lang.w3i.PATH]     = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.TEXT]     = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.TITLE]    = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.SUBTITLE] = w2l:load_wts(self.wts, (self:unpack 'z')),
        }

        chunk[lang.w3i.CONFIG][lang.w3i.GAME_DATA_SETTING] = self:unpack 'l'
    
        chunk[lang.w3i.PROLOGUE] = {
            [lang.w3i.PATH]     = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.TEXT]     = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.TITLE]    = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.SUBTITLE] = w2l:load_wts(self.wts, (self:unpack 'z')),
        }
    
        chunk[lang.w3i.FOG] = {
            [lang.w3i.TYPE]    = self:unpack 'l',
            [lang.w3i.START_Z] = self:unpack 'f',
            [lang.w3i.END_Z]   = self:unpack 'f',
            [lang.w3i.DENSITY] = self:unpack 'f',
            [lang.w3i.COLOR]   = pack(self:unpack 'BBBB'),
        }
        
        chunk[lang.w3i.ENVIRONMENT] = {
            [lang.w3i.WEATHER]     = self:unpack 'c4',
            [lang.w3i.SOUND]       = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.LIGHT]       = self:unpack 'c1',
            [lang.w3i.WATER_COLOR] = pack(self:unpack 'BBBB'),
        }
    elseif version == 18 then
        chunk[lang.w3i.LOADING_SCREEN] = {
            [lang.w3i.ID]          = self:unpack 'l',
            [lang.w3i.TEXT]        = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.TITLE]       = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.SUBTITLE]    = w2l:load_wts(self.wts, (self:unpack 'z')),
        }

        chunk[lang.w3i.PROLOGUE] = {
            [lang.w3i.ID]          = self:unpack 'l',
            [lang.w3i.TEXT]     = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.TITLE]    = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.SUBTITLE] = w2l:load_wts(self.wts, (self:unpack 'z')),
        }
    end
end

function mt:add_player(chunk)
    chunk[lang.w3i.PLAYER] = {
        [lang.w3i.PLAYER_COUNT] = self:unpack 'l',
    }

    for i = 1, chunk[lang.w3i.PLAYER][lang.w3i.PLAYER_COUNT] do
        chunk[lang.w3i.PLAYER..i] = {
            [lang.w3i.PLAYER]             = self:unpack 'l',
            [lang.w3i.TYPE]               = self:unpack 'l',
            [lang.w3i.RACE]               = self:unpack 'l',
            [lang.w3i.FIX_START_POSITION] = self:unpack 'l',
            [lang.w3i.NAME]               = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.START_POSITION]     = pack(self:unpack 'ff'),
            [lang.w3i.ALLY_LOW_FLAG]      = unpack_flag(self:unpack 'L'),
            [lang.w3i.ALLY_HIGH_FLAG]     = unpack_flag(self:unpack 'L'),
        }
    end
end

function mt:unpack_player_flag(chunk)
    local flag = self:unpack 'L'
    local tbl = unpack_flag(flag)
    local exits = {}
    for i = 1, chunk[lang.w3i.PLAYER][lang.w3i.PLAYER_COUNT] do
        local player = chunk[lang.w3i.PLAYER..i]
        local id = player[lang.w3i.PLAYER] + 1
        exits[id] = true
    end
    local result = {}
    for _, id in ipairs(tbl) do
        if exits[id] then
            result[#result+1] = id
        end
    end
    return result
end

function mt:add_force(chunk)
    chunk[lang.w3i.FORCE] = {
        [lang.w3i.FORCE_COUNT] = self:unpack 'l',
    }

    for i = 1, chunk[lang.w3i.FORCE][lang.w3i.FORCE_COUNT] do
        local flag = self:unpack 'L'
        chunk[lang.w3i.FORCE..i] = {
            [lang.w3i.ALLY]          = flag >> 0 & 1,
            [lang.w3i.ALLY_WIN]      = flag >> 1 & 1,
            [lang.w3i.SHARE_VISIBLE] = flag >> 3 & 1,
            [lang.w3i.SHARE_CONTROL] = flag >> 4 & 1,
            [lang.w3i.SHARE_ADVANCE] = flag >> 5 & 1,
            [lang.w3i.PLAYER_LIST]   = self:unpack_player_flag(chunk),
            [lang.w3i.FORCE_NAME]    = w2l:load_wts(self.wts, (self:unpack 'z')),
        }
    end
end

function mt:add_upgrade(chunk)
    if self:is_finish() then
        return
    end
    local count = self:unpack 'l'
    for i = 1, count do
        chunk[lang.w3i.UPGRADE..i] = {
            [lang.w3i.PLAYER_LIST] = unpack_flag(self:unpack 'L'),
            ['ID']                 = self:unpack 'c4',
            [lang.w3i.LEVEL]       = self:unpack 'l',
            [lang.w3i.AVAILABLE]   = self:unpack 'l',
        }
    end
end

function mt:add_tech(chunk)
    if self:is_finish() then
        return
    end
    local count = self:unpack 'l'
    for i = 1, count do
        chunk[lang.w3i.TECH..i] = {
            [lang.w3i.PLAYER_LIST] = unpack_flag(self:unpack 'L'),
            ['ID']                 = self:unpack 'c4',
        }
    end
end

function mt:add_randomgroup(chunk)
    if self:is_finish() then
        return
    end
    local count = self:unpack 'l'
    for i = 1, count do
        chunk[lang.w3i.RANDOM_GROUP..i] = {
            ['ID']                       = self:unpack 'l',
            [lang.w3i.RANDOM_GROUP_NAME] = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.SETTING]           = {},
        }
        local x = self:unpack 'l'
        chunk[lang.w3i.RANDOM_GROUP..i][lang.w3i.POSITION_TYPE] = pack(self:unpack(('l'):rep(x)))

        local y = self:unpack 'l'
        for y = 1, y do
            chunk[lang.w3i.RANDOM_GROUP..i][lang.w3i.SETTING][y] = {
                [lang.w3i.CHANCE] = self:unpack 'l',
                ['ID']            = pack(self:unpack(('c4'):rep(x))),
            }
        end
    end
end

function mt:add_randomitem(chunk)
    if self:is_finish() then
        return
    end
    local count = self:unpack 'l'
    for i = 1, count do
        chunk[lang.w3i.RANDOM_ITEM..i] = {
            ['ID']                      = self:unpack 'l',
            [lang.w3i.RANDOM_ITEM_NAME] = w2l:load_wts(self.wts, (self:unpack 'z')),
            [lang.w3i.SETTING]          = {},
        }

        --设置
        local x = self:unpack 'l'
        for x = 1, x do
            chunk[lang.w3i.RANDOM_ITEM..i][lang.w3i.SETTING][x] = {}
            local y = self:unpack 'l'
            for y = 1, y do
                chunk[lang.w3i.RANDOM_ITEM..i][lang.w3i.SETTING][x][y] = {
                    [lang.w3i.CHANCE] = self:unpack 'l',
                    ['ID']            = self:unpack 'c4',
                }
            end
        end
    end
end

return function (w2l_, content, wts)
    if not content then
        return nil
    end
    w2l = w2l_
    
    local index = 1
    local tbl   = setmetatable({}, mt)
    local data  = {}

    tbl.content = content
    tbl.index   = index
    tbl.wts     = wts

    local version = tbl:get_version(data)
    if version == 25 then
        tbl:add_head(data, version)
        tbl:add_player(data)
        tbl:add_force(data)
        tbl:add_upgrade(data)
        tbl:add_tech(data)
        tbl:add_randomgroup(data)
        tbl:add_randomitem(data)
    elseif version == 18 then
        tbl:add_head(data, version)
        tbl:add_player(data)
        tbl:add_force(data)
        tbl:add_upgrade(data)
        tbl:add_tech(data)
        tbl:add_randomgroup(data)
    end
    
    return data
end
