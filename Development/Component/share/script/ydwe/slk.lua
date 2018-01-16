local w3x2lni = require 'w3x2lni_in_sandbox'
local root = fs.ydwe_devpath()
local stormlib  = require 'ffi.stormlib'
local mpqloader = require 'mpqloader'
local i18n = require 'i18n'

local function get_config()
    local config = {}
    -- 转换后的目标格式(lni, obj, slk)
    config.target_format = 'obj'
    -- 使用的语言
    config.lang = i18n.get_language()

    return config
end

local function initialize(mappath)
    local map = stormlib.attach(mappath)
    if not map then
        return
    end
    local w2l = w3x2lni()
    local has_created
    w2l:set_config(get_config())
    local mpq_path = root / 'share' / 'mpq'
    function w2l:mpq_load(filename)
        return mpqloader:load(mpq_path, filename)
    end
    local prebuilt_path = root / 'share' / 'script' / 'ydwe' / 'prebuilt'
    function w2l:prebuilt_load(filename)
        local buf = mpqloader:load(prebuilt_path, filename)
        if not buf and not has_created then
            has_created = true
            require('prebuilt')(w2l, prebuilt_path)
            buf = mpqloader:load(prebuilt_path, filename)
        end
        return buf
    end
    function w2l:map_load(filename)
        return map:load_file(filename)
    end
    function w2l:map_save(filename, buf)
        return map:save_file(filename, buf)
    end
    function w2l:map_remove(filename)
        return map:remove_file(filename)
    end
    return w2l:slk_lib()
end

return initialize(__map_handle__.handle)
