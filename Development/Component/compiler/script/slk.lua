local w3x2lni = require 'compiler.w3x2lni.init'
local slk_lib = require 'compiler.w3x2lni.slk_lib'
local event = require 'ev'
local map_path = __map_path__

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

local function initialize()
    local w2l = w3x2lni()

    w2l.input_ar = {
        get = function (self, filename)
            log.info('load_file', filename)
            return io.load(map_path / filename)
        end,
    }
    w2l.output_ar = {
        set = function (self, filename, buf)
            if not type_map[filename] then
                return
            end
            import_files[filename] = buf
            log.info('Object save', filename)
        end,
        remove = function (self, filename)
            if not type_map[filename] then
                return
            end
            import_files[filename] = ('lll'):pack(2, 0, 0)
            log.info('Object remove', filename)
        end,
    }

    return slk_lib(w2l, false, true)
end

local trg
local slk = initialize()
trg = event.on('编译地图', function (success)
    package.loaded['slk'] = nil
    trg:remove()
    log.info('Flushed slk lib')
    if not success then
        return
    end
    import_files = {}
    log.trace('Refresh object start', map_path)
    local report = slk:refresh()
    log.trace('Refresh object finish')
    if #report > 0 then
        gui.message(nil, ('%s\n\n%s'):format('编辑器修改了物编数据', report))
        for filename, buf in pairs(import_files) do
            local file_path = map_path / filename
            log.info('Import customdata', filename, type_map[filename], file_path)
            io.save(file_path, buf)
            we.import_customdata(type_map[filename], file_path)
        end
    end
end)

return slk
