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

local function fmtstring(s)
    if s:find '[^%w_]' then
        return ('%q'):format(s)
    end
    return s
end

local function create_miscnames(w2l, keydata)
    local metadata = w2l:parse_slk(io.load(w2l.mpq / w2l.info.metadata['misc']))
    local names = {}
    keydata['misc_names'] = {}
    for id, meta in pairs(metadata) do
        local name = meta.section
        if not names[name] then
            names[name] = true
            table.insert(keydata['misc_names'], name)
        end
    end
end

local function stringify(f, name, t)
    if not t then
        return
    end
    f[#f+1] = ('%s = {'):format(fmtstring(name))
    table.sort(t)
    for _, v in ipairs(t) do
        f[#f+1] = ('%s,'):format(fmtstring(v))
    end
    f[#f+1] = '}'
end

return function(w2l)
    message('正在生成keydata')
    local keydata = {}
    create_miscnames(w2l, keydata)
    local f = {}
    f[#f+1] = '[root]'
    for k, v in sortpairs(keydata) do
        stringify(f, k, v)
    end
    io.save(w2l.defined / 'miscnames.ini', table.concat(f, '\r\n'))
end
