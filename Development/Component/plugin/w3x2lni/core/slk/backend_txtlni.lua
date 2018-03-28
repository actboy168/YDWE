
local function sortpairs(t)
    local sort = {}
    for k, v in pairs(t) do
        sort[#sort+1] = {k, v}
    end
    table.sort(sort, function (a, b)
        return a[1] < b[1]
    end)
    local n = 1
    return function()
        local v = sort[n]
        if not v then
            return
        end
        n = n + 1
        return v[1], v[2]
    end
end

local function format_value(value)
    if tonumber(value) then
        return tostring(value)
    else
        if value:match '[\n\r]' then
            return ('[=[\r\n%s]=]'):format(value)
        else
            return ('%q'):format(value)
        end
    end
end

local function maxindex(t)
    local i = 0
    for k in pairs(t) do
        i = math.max(i, k)
    end
    return i
end

local function write_data(f, k, v)
    if k:find '[^%w_]' then
        k = ('%q'):format(k)
    end
    if type(v) == 'table' then
        local l = {}
        for i = 1, maxindex(v) do
            l[i] = format_value(v[i]) or ''
        end
        if #l == 0 then
            return
        elseif#l == 1 then
            f[#f+1] = ('%s = %s'):format(k, l[1])
        else
            f[#f+1] = ('%s = {%s}'):format(k, table.concat(l, ', '))
        end
    else
        f[#f+1] = ('%s = %s'):format(k, format_value(v))
    end
end

local function is_enable(obj)
    for key, value in pairs(obj) do
        if key:sub(1, 1) ~= '_' then
            if type(value) == 'table' then
                if next(value) then
                    return true
                end
            else
                return true
            end
        end
    end
    return false
end

return function (w2l, t)
    if not t then
        return
    end
    local f = {}
    for i, o in sortpairs(t) do
        if is_enable(o) then
            f[#f+1] = ('[%s]'):format(i)
            for k, v in sortpairs(o) do
                if k:sub(1, 1) ~= '_' then
                    write_data(f, k, v)
                end
            end
            f[#f+1] = ''
        end
    end
    return #f > 0 and table.concat(f, '\r\n')
end
