local file_version = require 'ffi.file_version'
local stormlib = require 'ffi.stormlib'

local mpq_names = {
    'War3.mpq',
    'War3x.mpq',
    'War3xLocal.mpq',
    'War3Patch.mpq',
}

local function load_mpq(dir, filename)
    for i, name in ipairs(mpq_names) do
        local mpq = stormlib.open(dir / name, true)
        if mpq then
            local buf = mpq:load_file(filename)
            mpq:close()
            if buf then
                return buf
            end
        end
    end
    return nil
end

local language_map = {
    [0x00000409] = 'enUS',
    [0x00000809] = 'enGB',
    [0x0000040c] = 'frFR',
    [0x00000407] = 'deDE',
    [0x0000040a] = 'esES',
    [0x00000410] = 'itIT',
    [0x00000405] = 'csCZ',
    [0x00000419] = 'ruRU',
    [0x00000415] = 'plPL',
    [0x00000416] = 'ptBR',
    [0x00000816] = 'ptPT',
    [0x0000041f] = 'tkTK',
    [0x00000411] = 'jaJA',
    [0x00000412] = 'koKR',
    [0x00000404] = 'zhTW',
    [0x00000804] = 'zhCN',
    [0x0000041e] = 'thTH',
}

local function mpq_language(config)
    if not config then
        return nil
    end
    local id = config:match 'LANGID=(0x[%x]+)'
    if not id then
        return nil
    end
    return language_map[tonumber(id)]
end

local function war3_ver (input)
    if not fs.is_directory(input) then
        return nil
    end
    local exe_path = input / 'War3.exe'
    if fs.exists(exe_path) then
        local ver = file_version(exe_path:string())
        if ver.major > 1 or ver.minor >= 29 then
            return ('%d.%d.%d'):format(ver.major, ver.minor, ver.revision)
        end
    end
    local exe_path = input / 'Warcraft III.exe'
    if fs.exists(exe_path) then
        local ver = file_version(exe_path:string())
        if ver.major > 1 or ver.minor >= 29 then
            return ('%d.%d.%d'):format(ver.major, ver.minor, ver.revision)
        end
    end
    local dll_path = input / 'Game.dll'
    if fs.exists(dll_path) then
        local ver = file_version(dll_path:string())
        return ('%d.%d.%d'):format(ver.major, ver.minor, ver.revision)
    end
    return nil
end

return function (input)
    local war3_ver = war3_ver(input)
    if not war3_ver then
        return nil
    end
    
    local lg = mpq_language(load_mpq(input, 'config.txt'))
    if not lg then
        return nil
    end

    return lg .. '-' .. war3_ver
end
