local lang = require 'lang'
local w2l

local function unify(name)
    return name:lower():gsub('/', '\\'):gsub('\\[\\]+', '\\')
end

local function search_staticfile(map, callback)
    for _, name in ipairs {'(listfile)', '(signature)', '(attributes)'} do
        callback(name)
    end
end

local function search_listfile(map, callback)
    local buf = map:get '(listfile)'
    if buf then
        for name in buf:gmatch '[^\r\n]+' do
            callback(name)
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
            name = unify(name)
            if mark[name] then
                return
            end
            mark[name] = true
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

    if count ~= total then
        w2l.messager.report(lang.report.ERROR, 1, lang.report.FILE_LOST:format(total - count), lang.report.FILE_LOST_HINT)
        w2l.messager.report(lang.report.ERROR, 1, lang.report.FILE_READ:format(count, total))
    end

    return files
end

local ignore = {}
for _, name in ipairs {'.git', '.svn', '.vscode', '.gitignore'} do
    ignore[name] = true
end

local function scan_dir(dir, callback)
    for path in dir:list_directory() do
        if not ignore[path:filename():string()] then
            if fs.is_directory(path) then
                scan_dir(path, callback)
            else
                callback(path)
            end
        end
    end
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
