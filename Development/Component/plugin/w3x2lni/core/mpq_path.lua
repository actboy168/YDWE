return function ()
    local mt = {}
    local archive = nil
    local lang = nil
    function mt:lang(lng)
        lang = lng
    end
    function mt:open(path)
        archive = path
    end
    function mt:close(path)
        if archive == path then
            archive = nil
        end
    end
    function mt:each_path(callback)
        if lang then
            if archive then
                local buf = callback(lang .. '\\' .. archive)
                if buf then
                    return buf
                end
            end
            local buf = callback(lang)
            if buf then
                return buf
            end
        end
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
    function mt:first_path()
        if lang then
            if archive then
                return lang .. '\\' .. archive
            end
            return lang
        end
        if archive then
            return archive
        end
        return ''
    end
    return mt
end
