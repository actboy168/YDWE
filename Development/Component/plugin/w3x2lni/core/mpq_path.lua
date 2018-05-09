return function ()
    local mt = {}
    local paths = {''}
    local mpqs = {}
    local function update()
        paths = {''}
        for i = #mpqs, 1, -1 do
            local path = mpqs[i]
            local max = #paths
            table.insert(paths, path)
            for i = 2, max do
                table.insert(paths, path .. '\\' .. paths[i])
            end
        end
    end
    function mt:open(path)
        table.insert(mpqs, path)
        update()
    end
    function mt:close(path)
        for i, mpq in ipairs(mpqs) do
            if mpq == path then
                table.remove(mpqs, i)
                update()
                return
            end
        end
    end
    function mt:each_path(callback)
        for i = #paths, 1, -1 do
            local res = callback(paths[i])
            if res then
                return res
            end
        end
    end
    function mt:first_path()
        return paths[#paths]
    end
    return mt
end
