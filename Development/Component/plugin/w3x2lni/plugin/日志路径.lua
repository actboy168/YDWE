local mt = {}

mt.info = {
    name = '日志路径',
    version = 1.0,
    author = '最萌小汐',
    description = '不要将日志生成到开发目录中。',
}

function mt:on_convert(w2l)
    local fs = require 'fs'
    w2l.log_path = fs.ydwe(false) / 'logs' / 'w3x2lni'
end

return mt
