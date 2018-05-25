local root = fs.ydwe_path()
local event = require 'ev'
local w3x2lni = require 'w3x2lni'
local check_lni_mark = loadfile((root / 'plugin' / 'w3x2lni' / 'script' / 'share' / 'check_lni_mark.lua'):string())()

local function input_ar(dir)
    local ar = {}

    function ar:get(name)
        log.info('input_ar', name)
        return io.load(dir / name)
    end

    return ar
end

local function output_ar(dir)
    local files = {}
    local ar = {}

    function ar:get(name)
        return files[name]
    end

    function ar:set(name, buf)
        files[name] = buf
        log.info('output_ar', name, #buf)
    end

    function ar:remove(name)
        files[name] = nil
    end
    
    return ar
end

return function ()
    event.on('virtual_mpq: open map', function(mappath)
        local path = fs.path(mappath)
        if path:filename():string() ~= '.w3x' then
            return
        end
        local buf = io.load(path)
        if not check_lni_mark(buf) then
            return
        end

        local dir = path:parent_path()
        local w2l = w3x2lni()
        w2l:set_setting { mode = 'lni' }
        w2l.input_ar = input_ar(dir)
        w2l.output_ar = output_ar(dir)
        w2l:frontend()
        w2l:backend()
    end)
end
