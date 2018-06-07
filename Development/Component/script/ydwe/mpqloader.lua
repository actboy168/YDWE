local event = require 'ev'
local ydwe = fs.ydwe_devpath()
local lang = (require "i18n").get_language()
local path

event.on('virtual_mpq: open path', function(name)
    log.info('OpenPathAsArchive', name)
    path = name
end)
event.on('virtual_mpq: close path', function(name)
    log.info('ClosePathAsArchive', name)
    path = nil
end)

local mt = {}

function mt:load(filename)
    if filename:sub(1, 5) == 'ydwe/' then
        local res = io.load(ydwe / 'share' / lang / filename)
        if res then
            return res
        end
        if lang ~= 'zh-CN' then
            local res = io.load(ydwe / 'share' / 'zh-CN' / filename)
            if res then
                return res
            end
        end
        return
    end
    if path then
        local res = io.load(ydwe / 'share' / lang / 'mpq' / path / filename)
        if res then
            return res
        end
    end
    local res = io.load(ydwe / 'share' / lang /  'mpq' /filename)
    if res then
        return res
    end
    if lang ~= 'zh-CN' then
        if path then
            local res = io.load(ydwe / 'share' / 'zh-CN' / 'mpq' / path / filename)
            if res then
                return res
            end
        end
        local res = io.load(ydwe / 'share' / 'zh-CN' / 'mpq' / filename)
        if res then
            return res
        end
    end
end

return mt
