local w3x2lni = require 'w3x2lni_in_sandbox'
local root = fs.ydwe_devpath()
local stormlib  = require 'ffi.stormlib'
local mpqloader = require 'mpqloader'
local i18n = require 'i18n'
local event = require 'ev'
local map_handle = __map_handle__.handle

local function initialize()
    local map = stormlib.attach(map_handle)
    if not map then
        return
    end

    local w2l = w3x2lni()

    local mpq_path = root / 'share' / 'mpq'
    function w2l:mpq_load(filename)
        return mpqloader:load(mpq_path, filename)
    end

    local prebuilt_path = root / 'share' / 'script' / 'ydwe' / 'prebuilt'
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
        log.debug('map_save', filename, #buf)
        return map:save_file(filename, buf)
    end
    function w2l:map_remove(filename)
        return map:remove_file(filename)
    end

    return w2l:slk_lib(false, true)
end

local trg
local slk = initialize()
trg = event.on('编译地图', function ()
    package.loaded['slk'] = nil
    trg:remove()
    log.trace('build object start', map_handle)
    slk:refresh(function(msg)
        log.trace('build object finish')
        if #msg == 0 then
            return
        end
        gui.message(nil, ('%s\n\n%s'):format('编辑器刚刚帮你修改了物编数据,建议重新打开地图,以便查看变化', msg))
    end)
end)

return slk
