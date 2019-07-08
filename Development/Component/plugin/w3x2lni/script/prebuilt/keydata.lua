local messager = require 'share.messager'
local lang = require 'share.lang'
local loader
local key_cache = {}
local function get_key(w2l, type, id)
    if not key_cache[type] then
        local t = {}
        for key, meta in pairs(w2l:metadata()[type]) do
            t[meta.id] = key
        end
        key_cache[type] = t
    end
    return key_cache[type][id]
end

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

local function is_enable(meta, type)
    if type == 'unit' then
        if meta.useHero == 1 or meta.useUnit == 1 or meta.useBuilding == 1 or meta.useCreep == 1 then
            return true
        else
            return false
        end
    end
    if type == 'item' then
        if meta.useItem == 1 then
            return true
        else
            return false
        end
    end
    return true
end

local function create_keydata(w2l, type, keydata)
    local metadata = w2l:parse_slk(loader('units\\' .. w2l.info.metadata[type]) or loader('doodads\\' .. w2l.info.metadata[type]))
    metadata.Ytip = nil
    for id, meta in pairs(metadata) do
        if is_enable(meta, type) and not meta.useSpecific or meta.section then
            local key = get_key(w2l, type, id)
            local filename = meta.slk:lower()
            if filename == 'profile' then
                filename = type
            else
                filename = 'units\\' .. filename .. '.slk'
                if type == 'doodad' then
                    filename = 'doodads\\doodads.slk'
                end
            end
            if not keydata[filename] then
                keydata[filename] = {}
            end
            table.insert(keydata[filename], key)
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

return function(w2l, loader_)
    loader = loader_
    messager.text(lang.raw.CREATING .. 'keydata')
    local keydata = {}
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc'} do
        create_keydata(w2l, type, keydata)
    end
    local f = {}
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc'} do
        stringify(f, type, keydata[type])
        keydata[type] = nil
    end
    for k, v in sortpairs(keydata) do
        stringify(f, k, v)
    end
    return table.concat(f, '\r\n')
end
