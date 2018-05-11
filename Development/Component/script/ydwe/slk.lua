local w3x2lni = require 'w3x2lni'
local root = fs.ydwe_devpath()
local ydpath = fs.ydwe_path()
local stormlib  = require 'ffi.stormlib'
local mpqloader = require 'mpqloader'
local i18n = require 'i18n'
local event = require 'ev'
local slk_lib = require 'slk_lib'
local defined = require 'defined'
local map_handle = __map_handle__.handle

local type_map = {
    ['war3map.w3u'] = 0,
    ['war3map.w3t'] = 1,
    ['war3map.w3b'] = 2,
    ['war3map.w3d'] = 3,
    ['war3map.w3a'] = 4,
    ['war3map.w3h'] = 5,
    ['war3map.w3q'] = 6,
}

local import_files
local map

local function initialize()
    map = stormlib.attach(map_handle)
    if not map then
        return
    end

    local w2l = w3x2lni()

    local mpq_path = root / 'share' / 'mpq'
    function w2l:mpq_load(filename)
        return mpqloader:load(mpq_path, filename)
    end

    local defined_path = ydpath / 'share' / 'mpq' / 'defined'
    function w2l:defined_load(filename)
        return io.load(defined_path / filename)
    end

    defined(w2l)

    local prebuilt_path = ydpath / 'script' / 'ydwe' / 'prebuilt'
    function w2l:prebuilt_load(filename)
        return mpqloader:load(prebuilt_path, filename)
    end
    function w2l:prebuilt_save(filename, buf)
        mpqloader:save(prebuilt_path, filename, buf)
    end

    function w2l:map_load(filename)
        return map:load_file(filename)
    end
    function w2l:map_save(filename, buf)
        import_files[filename] = buf
        log.info('Object save', filename)
    end
    function w2l:map_remove(filename)
        import_files[filename] = ('lll'):pack(2, 0, 0)
        log.info('Object remove', filename)
    end

    return slk_lib(w2l, false, true)
end

local trg
local slk = initialize()
trg = event.on('编译地图', function ()
    package.loaded['slk'] = nil
    trg:remove()
    import_files = {}
    log.trace('Refresh object start', map_handle)
    local report = slk:refresh()
    log.trace('Refresh object finish')
    if #report > 0 then
        gui.message(nil, ('%s\n\n%s'):format('编辑器修改了物编数据', report))
        for filename, buf in pairs(import_files) do
            local tmp = fs.path(os.tmpname()):remove_filename() / filename
            log.info('Import customdata', filename, type_map[filename], tmp)
            io.save(tmp, buf)
            we.import_customdata(type_map[filename], tmp)
            map:save_file(filename, buf)
        end
    end
end)

return slk
