local mt = {}

mt.info = {
    name = '日志路径',
    version = 1.0,
    author = '最萌小汐',
}

local function ydwePath(support_dev)
    local ydwe = fs.module_path():parent_path():parent_path()
    if support_dev then
        local ydwedev = ydwe:parent_path():parent_path():parent_path()
        if fs.exists(ydwedev / "build.root") then
            return ydwedev / "Component"
        end
    end
    return ydwe
end

function mt:on_convert(w2l)
    w2l.log_path = ydwePath(false) / 'logs' / 'w3x2lni'
end

function mt:on_pack(w2l)
    w2l.log_path = ydwePath(false) / 'logs' / 'w3x2lni'
end

return mt
