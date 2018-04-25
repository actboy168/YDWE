return function (path)
    local map = io.open(path:string(), 'rb')
    if map then
        map:seek('set', 8)
        local mark = map:read(4)
        map:close()
        if mark == 'W2L\x01' then
            return true
        end
    end
    return false
end
