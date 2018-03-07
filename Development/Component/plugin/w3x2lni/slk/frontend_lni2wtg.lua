local w2l

local function sorted_table(tbl)
    if not tbl then
        tbl = {}
    end
    local keys = {}
    local mark = {}
    local mt = {}

    function mt:__newindex(k, v)
        rawset(self, k, v)
        if type(v) == 'table' then
            sorted_table(v)
        end
        if not mark[k] then
            mark[k] = true
            keys[#keys+1] = k
        end
    end

    function mt:__pairs()
        local i = 0
        return function ()
            i = i + 1
            local key = keys[i]
            return key, self[key]
        end
    end

    return setmetatable(tbl, mt)
end

local function convert_wtg()
end

local function convert_wct(files)
    local wct = {}

    local list = w2l:parse_lni(files['.目录.ini'], '.目录.ini')

    return wct
end

return function (w2l_, files)
    w2l = w2l_
    
    local wtg = convert_wtg(files)
    local wct = convert_wct(files)

    return wtg, wct
end
