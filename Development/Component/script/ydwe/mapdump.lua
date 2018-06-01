local function tohex(n)
    return (n & 0xFF)
    ..' '..((n >> 8) & 0xFF)
    ..' '..((n >> 16) & 0xFF)
    ..' '..((n >> 24) & 0xFF)
end

local function tohex2(n)
    return (n & 0xFF)
    ..' '..((n >> 8) & 0xFF)
end

return function (input, jass, write)
    local maphash = require 'maphash'
    local size, info, crc, sha1_a, sha1_b, sha1_c, sha1_d, sha1_e = maphash(input, jass)
    write('map_size = ' .. tohex(size))
    write('map_info = ' .. tohex(info))
    write('map_crc = ' .. tohex(crc))
    write('map_sha1 = ' .. tohex(sha1_a)..' '..tohex(sha1_b)..' '..tohex(sha1_c)..' '..tohex(sha1_d)..' '..tohex(sha1_e))

    local stormlib = require 'ffi.stormlib'
    local map = stormlib.open(input, true)
    if not map then
        return error('Couldn\'t open map.')
    end
    local w3x2lni = require 'w3x2lni.init'
    local w2l = w3x2lni()
    local info = w2l:frontend_w3i(map:load_file 'war3map.w3i')
    map:close()
    local map_options = (info.CONFIG.MELEE_MAP << 2)
        | (info.CONFIG.FIX_FORCE_SETTING << 5)
        | (info.CONFIG.CUSTOM_FORCE << 6)
    write('map_options = ' .. map_options)
    write('map_width = ' .. tohex2(info.MAP_INFO.MAP_WIDTH))
    write('map_height = ' .. tohex2(info.MAP_INFO.MAP_HEIGHT))
    local n = info.PLAYER.PLAYER_COUNT
    local players = {}
    for i = 1, n do
        local t = info['PLAYER'..i]
        if t.TYPE == 1 or t.TYPE == 2 then
            local ply = {}
            ply.pid = 0
            ply.download_status = 255
            if t.TYPE == 1 then
                ply.slot_status = 0
                ply.computer = 0
            elseif t.TYPE == 2 then
                ply.slot_status = 2
                ply.computer = 1
            else
                ply.slot_status = 1
                ply.computer = 0
            end
            ply.colour = t.PLAYER
            if t.RACE == 1 then
                -- human
                ply.race = 1
            elseif t.RACE == 2 then
                -- orc
                ply.race = 2
            elseif t.RACE == 3 then
                -- undead
                ply.race = 8
            elseif t.RACE == 4 then
                -- nightelf
                ply.race = 4
            else
                -- random
                ply.race = 32
            end
            ply.computer_type = 1
            ply.handicap = 100

            table.insert(players, ply)
        end
    end
    for i = 1, info.FORCE.FORCE_COUNT do
        for _, c in ipairs(info['FORCE'..i].PLAYER_LIST) do
            for _, ply in ipairs(players) do
                if ply.colour + 1 == c then
                    ply.team = i - 1
                    break
                end
            end
        end
    end
    for i, ply in ipairs(players) do
        write(('map_slot%d = %d %d %d %d %d %d %d %d %d'):format(i
            , ply.pid
            , ply.download_status
            , ply.slot_status
            , ply.computer
            , ply.team
            , ply.colour
            , ply.race
            , ply.computer_type
            , ply.handicap
            ))
    end
end
