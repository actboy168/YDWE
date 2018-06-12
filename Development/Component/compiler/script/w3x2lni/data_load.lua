require 'filesystem'
local ydwePath = fs.ydwe(true)
local w2lPath = ydwePath / 'plugin' / 'w3x2lni'
local currentLanguage = (require "i18n").get_language()

return function (w2l, filename)
    if filename:sub(1, 3) == 'ui/' then
        return io.load(ydwePath / filename)
    end
    if filename:sub(1, 14) == 'data/prebuilt/' then
        return io.load(w2lPath / 'data' / w2l.setting.data / filename:sub(6))
    end
    if filename:sub(1, 5) == 'data/' then
        return io.load(ydwePath / 'share' / currentLanguage / filename:sub(6))
    end
    return io.load(w2lPath / filename)
end
