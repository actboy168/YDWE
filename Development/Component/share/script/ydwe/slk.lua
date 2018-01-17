local w3x2lni = require 'w3x2lni_in_sandbox'
local root = fs.ydwe_devpath()
local stormlib  = require 'ffi.stormlib'
local mpqloader = require 'mpqloader'
local i18n = require 'i18n'

local function initialize(mappath)
    local map = stormlib.attach(mappath)
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
        return map:save_file(filename, buf)
    end

    return w2l:slk_lib(false, true)
end

return initialize(__map_handle__.handle)
