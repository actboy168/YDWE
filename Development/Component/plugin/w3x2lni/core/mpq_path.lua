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
        return callback(archive) or callback('')
    end
    function mt:first_path()
        return archive or ''
    end
    return mt
end
