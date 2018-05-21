return function (buf)
    if not buf then
        return false
    end
    if buf:sub(9, 12) == 'W2L\x01' then
        return true
    end
    return false
end
