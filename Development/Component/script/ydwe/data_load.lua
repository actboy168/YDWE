require 'filesystem'
local root = fs.ydwe_devpath()
local w2l_path = root / 'plugin' / 'w3x2lni'

return function (w2l, filename)
    if filename:sub(1, 6) == 'ui/' and w2l.setting.data_ui == '${YDWE}' then
        return io.load(root / filename)
    end
    return io.load(w2l_path / 'data' / w2l.setting.data / filename)
end
