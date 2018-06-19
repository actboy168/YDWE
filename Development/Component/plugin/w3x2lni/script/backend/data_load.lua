require 'utility'
local root = require 'backend.w2l_path'

local function ydwe_ui(w2l)
    local ydwe_path = require 'backend.ydwe_path'
    local ydwe = ydwe_path()
    if not ydwe then
        return nil, lang.script.NEED_YDWE_ASSOCIATE
    end
    if fs.exists(ydwe / 'ui') then
        return ydwe / 'ui'
    elseif fs.exists(ydwe / 'share' / 'ui') then
        return ydwe / 'share' / 'ui'
    elseif fs.exists(ydwe / 'share' / 'mpq') then
        return ydwe / 'share' / 'mpq'
    else
        return nil, lang.script.NO_TRIGGER_DATA
    end
end

return function (w2l, filename)
    if filename:sub(1, 3) == 'ui\\' and w2l.setting.data_ui == '${YDWE}' then
        if not w2l.ydwe_ui then
            w2l.ydwe_ui = { ydwe_ui() }
        end
        if not w2l.ydwe_ui[1] then
            return nil, w2l.ydwe_ui[2]
        end
        return io.load(w2l.ydwe_ui[1] / filename:sub(4))
    end
    return io.load(root / 'data' / w2l.setting.data / filename)
end
