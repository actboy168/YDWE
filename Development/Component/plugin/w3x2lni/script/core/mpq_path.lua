return function ()
    local mt = {}
    local archive = nil
    function mt:open(path)
        archive = path
    end
    function mt:close(path)
        if archive == path then
            archive = nil
        end
    end
    function mt:each_path(callback)
        if archive then
            local buf = callback(archive)
            if buf then
                return buf
            end
        end
        local buf = callback('')
        if buf then
            return buf
        end
    end
    return mt
end
