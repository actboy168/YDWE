local lang = require 'lang'

local w2l
local wtg
local wct

local type = type
local tonumber = tonumber
local pairs = pairs
local ipairs = ipairs
local find = string.find
local gsub = string.gsub
local format = string.format
local rep = string.rep
local buf
local lml_table

local sp_rep = setmetatable({}, {
    __index = function (self, k)
        self[k] = rep(' ', k)
        return self[k]
    end,
})

local function lml_string(str)
    if type(str) == 'string' then
        if find(str, "[%s%:%'%c]") then
            str = format("'%s'", gsub(str, "'", "''"))
        end
    end
    return str
end

local function lml_value(v, sp)
    if v[2] then
        buf[#buf+1] = format('%s%s: %s\n', sp_rep[sp], lml_string(v[1]), lml_string(v[2]))
    else
        buf[#buf+1] = format('%s%s\n', sp_rep[sp], lml_string(v[1]))
    end
    for i = 3, #v do
        lml_value(v[i], sp+4)
    end
end

local function convert_lml(tbl)
    buf = {}
    for i = 3, #tbl do
        lml_value(tbl[i], 0)
    end
    return table.concat(buf)
end

local function get_path(path, used)
    path = path:gsub('[$\\$/$:$*$?$"$<$>$|]', '_')
    while used[path] do
        local name, id = path:match '(.+)_(%d+)$'
        if name and id then
            id = id + 1
        else
            name = path
            id = 1
        end
        path = name .. '_' .. id
    end
    return path
end

local function compute_path()
    if not wtg then
        return
    end
    local map = {}
    map[1] = {}
    local dirs = {}
    for _, dir in ipairs(wtg.categories) do
        dirs[dir.id] = {}
        map[1][dir.name] = get_path(dir.name, map[1])
    end
    for _, trg in ipairs(wtg.triggers) do
        table.insert(dirs[trg.category], trg)
    end
    for _, dir in ipairs(wtg.categories) do
        map[dir.name] = {}
        for _, trg in ipairs(dirs[dir.id]) do
            map[dir.name][trg.name] = get_path(trg.name, map[dir.name])
        end
    end
    return map
end

local function read_dirs(map)
    local dirs = {}
    for _, dir in ipairs(wtg.categories) do
        dirs[dir.id] = {}
    end
    for _, trg in ipairs(wtg.triggers) do
        table.insert(dirs[trg.category], trg)
    end
    local lml = { '', false }
    for i, dir in ipairs(wtg.categories) do
        local filename = map[1][dir.name]
        local dir_data = { filename, dir.id }
        if dir.name ~= filename then
            dir_data[#dir_data+1] = { lang.lml.NAME, dir.name }
        end
        if dir.comment == 1 then
            dir_data[#dir_data+1] = { lang.lml.COMMENT, 1 }
        end
        for i, trg in ipairs(dirs[dir.id]) do
            local filename = map[dir.name][trg.name]
            local trg_data = { filename, false }
            if trg.name ~= filename then
                trg_data[#trg_data+1] = { lang.lml.NAME, trg.name }
            end
            if trg.type == 1 then
                trg_data[#trg_data+1] = { lang.lml.COMMENT }
            end
            if trg.enable == 0 then
                trg_data[#trg_data+1] = { lang.lml.DISABLE }
            end
            if trg.close == 1 then
                trg_data[#trg_data+1] = { lang.lml.CLOSE }
            end
            if trg.run == 1 then
                trg_data[#trg_data+1] = { lang.lml.RUN }
            end
            dir_data[#dir_data+1] = trg_data
        end
        lml[i+2] = dir_data
    end
    return convert_lml(lml)
end

local function read_triggers(files, map)
    if not wtg then
        return
    end
    local triggers = {}
    local dirs = {}
    for _, dir in ipairs(wtg.categories) do
        dirs[dir.id] = dir.name
    end
    for i, trg in ipairs(wtg.triggers) do
        local dir = dirs[trg.category]
        local path = map[1][dir] .. '\\' .. map[dir][trg.name]
        if trg.wct == 0 and trg.type == 0 then
            files[path..'.lml'] = convert_lml(trg.trg)
        end
        if #trg.des > 0 then
            files[path..'.txt'] = trg.des
        end
        if trg.wct == 1 then
            local buf = wct.triggers[i]
            if #buf > 0 then
                files[path..'.j'] = buf
            end
        end
    end
end

return function (w2l_, wtg_, wct_)
    w2l = w2l_
    wtg = wtg_
    wct = wct_

    local files = {}

    if #wct.custom.comment > 0 then
        files[lang.lml.CODE .. '.txt'] = wct.custom.comment
    end
    if #wct.custom.code > 0 then
        files[lang.lml.CODE .. '.j'] = wct.custom.code
    end

    local vars = convert_lml(wtg.vars)
    if #vars > 0 then
        files[lang.lml.VARIABLE .. '.lml'] = vars
    end

    local map = compute_path()
    
    local listfile = read_dirs(map)
    if #listfile > 0 then
        files[lang.lml.CATALOG .. '.lml'] = listfile
    end

    read_triggers(files, map)

    return files
end
