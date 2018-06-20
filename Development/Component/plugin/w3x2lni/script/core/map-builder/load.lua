local lang = require 'lang'
local w2l

local function search_staticfile(map, callback)
    local function search_tbl(tbl)
        for _, v in pairs(tbl) do
            if type(v) == 'table' then
                search_tbl(v)
            elseif type(v) == 'string' then
                callback(v)
            end
        end
    end
    search_tbl(w2l.info)
end

local function search_listfile(map, callback)
    local buf = map:get '(listfile)'
    if buf then
        local start = 1
        while true do
            local pos = buf:find('\r\n', start)
            if not pos then
                callback(buf:sub(start))
                break
            end
            callback(buf:sub(start, pos-1))
            start = pos + 2
        end
    end
end

local function search_imp(map, callback)
    local buf = map:get 'war3map.imp'
    if buf then
        local _, count, index = ('ll'):unpack(buf)
        local name
        for i = 1, count do
            _, name, index = ('c1z'):unpack(buf, index)
            callback(name)
            if not map:has(name) then
                local name = 'war3mapimported\\' .. name
                callback(name)
            end
        end
    end
end

local searchers = {
    search_listfile,
    search_staticfile,
    search_imp,
}

local function search_mpq(map)
    local total = map:number_of_files()
    local count = 0
    local clock = os.clock()
    local mark = {}
    local files = {}
    for i, searcher in ipairs(searchers) do
        pcall(searcher, map, function (name)
            local lname = name:lower()
            if mark[lname] then
                return
            end
            mark[lname] = true
            if not map:has(name) then
                return
            end
            files[name] = map:get(name)
            count = count + 1
            if os.clock() - clock > 0.1 then
                clock = os.clock()
                w2l.messager.text(lang.script.LOAD_MAP_FILE:format(count, total))
                w2l.progress(count / total)
            end
        end)
    end

    return files
end

local function search_dir(map)
    local list = map:list_file()
    local total = #list
    local clock = os.clock()
    local count = 0
    local files = {}
    for i, name in ipairs(list) do
        files[name] = map:get(name)
        count = count + 1
        if os.clock() - clock > 0.1 then
            clock = os.clock()
            w2l.messager.text(lang.script.LOAD_MAP_FILE:format(count, total))
            w2l.progress(count / total)
        end
    end
    return files
end

return function (w2l_, map)
    w2l = w2l_
    if map:get_type() == 'mpq' then
        return search_mpq(map)
    else
        return search_dir(map)
    end
end
