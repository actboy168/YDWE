local root = fs.ydwe_path()
local event = require 'ev'
local w3x2lni = require 'w3x2lni'
local check_lni_mark = loadfile((root / 'plugin' / 'w3x2lni' / 'script' / 'share' / 'check_lni_mark.lua'):string())()

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

local function unify(name)
    return name:lower():gsub('/', '\\'):gsub('\\[\\]+', '\\')
end

local function search_dir(dir)
    local files = {}
    local len = #dir:string()
    for _, dir_name in ipairs {'map', 'table', 'resource', 'scripts', 'sound', 'trigger', 'w3x2lni'} do
        scan_dir(dir / dir_name, function(path)
            local name = unify(path:string():sub(len+2))
            files[name] = io.load(path)
            log.info('dummy map set', name)
        end)
    end
    return files
end

local function dummy_map_ar(dir)
    local files = search_dir(dir)
    local ar = {}

    function ar:get(name)
        return files[name]
    end
    
    function ar:set(name, buf)
        log.info('dummy map set', name)
        files[name] = buf
    end

    function ar:remove(name)
        log.info('dummy map remove', name)
        files[name] = nil
    end

    function ar:search_files()
        local tbl = {}
        for k, v in pairs(files) do
            tbl[k] = v
        end
        return pairs(tbl)
    end

    return ar
end

local function build_imp(w2l, output_ar, imp_buf)
    local impignore = {}
    for _, name in ipairs(w2l.info.pack.impignore) do
        impignore[name] = true
    end
    for _, name in pairs(w2l.info.obj) do
        impignore[name] = true
    end
    for _, name in pairs(w2l.info.lni) do
        impignore[name] = true
    end
    for _, slks in pairs(w2l.info.slk) do
        for _, name in ipairs(slks) do
            impignore[name] = true
        end
    end
    for _, name in ipairs(w2l.info.txt) do
        impignore[name] = true
    end
    local imp = {}
    for name, buf in output_ar:search_files() do
        if buf and not impignore[name] then
            imp[#imp+1] = name
        end
    end
    if imp_buf then
        local imp_lni = w2l:parse_lni(imp_buf, filename)
        for _, name in ipairs(imp_lni.import) do
            local name = name:lower()
            if impignore[name] then
                imp[#imp+1] = name
            end
        end
    end
    table.sort(imp)
    local hex = {}
    hex[1] = ('ll'):pack(1, #imp)
    for _, name in ipairs(imp) do
        hex[#hex+1] = ('z'):pack(name)
    end
    return table.concat(hex, '\r')
end

local function save_map(w2l, dummy_map)
    if not w2l:file_load('map', 'war3mapunits.doo') then
        w2l:file_save('map', 'war3mapunits.doo', w2l:backend_unitsdoo())
    end
    for _, name in pairs(w2l.info.pack.packignore) do
        w2l:file_remove('map', name)
    end
    local imp = w2l:file_load('table', 'imp')
    w2l:file_remove('table', 'imp')

    for type, name, buf in w2l:file_pairs() do
        w2l:file_save(type, name, buf)
    end

    if imp then
        w2l:file_save('map', 'war3map.imp', build_imp(w2l, dummy_map, imp))
    end
end

return function ()
    local dummy_map
    local ignore_once = nil
    event.on('virtual_mpq: open map', function(mappath)
        if ignore_once == mappath then
            ignore_once = nil
            return
        end
        ignore_once = mappath
        dummy_map = nil
        local path = fs.path(mappath)
        if path:filename():string() ~= '.w3x' then
            return
        end
        local buf = io.load(path)
        if not check_lni_mark(buf) then
            return
        end

        log.info('Open Lni map', path)
        local dir = path:parent_path()
        dummy_map = dummy_map_ar(dir)
        
        local w2l = w3x2lni()
        w2l.input_mode = 'lni'
        w2l:set_setting { mode = 'obj' }

        w2l.input_ar = dummy_map
        w2l.output_ar = dummy_map
        w2l:frontend()
        w2l:backend()
        save_map(w2l, dummy_map)
        log.info('Converted to Obj map')
    end)
    virtual_mpq.map_has(function (filename)
        if dummy_map then
            local buf = dummy_map:get(filename)
            if buf then
                return true
            end
        end
    end)
    virtual_mpq.map_load(function (filename)
        if dummy_map then
            local buf = dummy_map:get(filename)
            if not buf then
                return
            end
            dummy_map:remove(filename)
            return buf
        end
    end)
end
