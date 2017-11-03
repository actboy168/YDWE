
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
	local w2l = require 'w3x2lni'
    local info = w2l:read_w3i(map:load_file 'war3map.w3i')
    map:close()
    local map_options = (info['选项']['对战地图'] << 2)
        | (info['选项']['自定义玩家分组'] << 5)
        | (info['选项']['自定义队伍'] << 6)
    write('map_options = ' .. map_options)
    write('map_width = ' .. tohex2(info['地形']['地图宽度']))
    write('map_height = ' .. tohex2(info['地形']['地图长度']))
    local n = info['玩家']['玩家数量']
    local players = {}
    for i = 1, n do
        local t = info['玩家'..i]
        if t['类型'] == 1 or t['类型'] == 2 then
            local ply = {}
            ply.pid = 0
            ply.download_status = 255
            if t['类型'] == 1 then
                ply.slot_status = 0
                ply.computer = 0
            elseif t['类型'] == 2 then
                ply.slot_status = 2
                ply.computer = 1
            else
                ply.slot_status = 1
                ply.computer = 0
            end
            ply.colour = t['玩家']
            if t['种族'] == 1 then
                -- human
                ply.race = 1
            elseif t['种族'] == 2 then
                -- orc
                ply.race = 2
            elseif t['种族'] == 3 then
                -- undead
                ply.race = 8
            elseif t['种族'] == 4 then
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
    for i = 1, info['队伍']['队伍数量'] do
        for _, c in ipairs(info['队伍'..i]['玩家列表']) do
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
