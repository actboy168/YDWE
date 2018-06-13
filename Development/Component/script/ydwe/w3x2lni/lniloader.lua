local w3x2lni = require 'compiler.w3x2lni.init'
local stormlib = require 'ffi.stormlib'
local storm = require 'virtual_storm'
local root = fs.ydwe_path()

local ignore = {}
for _, name in ipairs {'.git', '.svn', '.vscode', '.gitignore'} do
    ignore[name] = true
end

local function is_lni(path)
    if path:filename():string() ~= '.w3x' then
        return false
    end
    local f = io.open(path)
    if not f then
        return false
    end
    if f:seek('set', 8) and 'W2L\x01' == f:read(4) then
        f:close()
        return true
    end
    f:close()
    return false
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

local function tga_mpq(map)
    local tgas = {}
    for name in pairs(map) do
        if name:sub(-4) == '.tga' then
            tgas[#tgas+1] = name
        end
    end
    local path = fs.path(os.tmpname()):parent_path() / 'tga.mpq'
    fs.remove(path)
    local mpq = stormlib.create(path, #tgas)
    for _, name in ipairs(tgas) do
        mpq:save_file(name, map:get(name))
    end
    mpq:close()
    log.info('Create tga mpq at ' .. path:string())
    return path
end

local function unify(name)
    return name:lower():gsub('/', '\\')
end

local function dummy_map_ar(dir)
    local files = {}
    local output = {}
    local ar = {}
    setmetatable(ar, ar)

    function ar:get(name)
        if files[name] == nil then
            files[name] = io.load(dir / name) or false
        end
        if files[name] == false then
            return nil
        end
        return files[name]
    end
    
    function ar:set(name, buf)
        files[name] = buf
        output[name] = buf
    end

    function ar:remove(name)
        files[name] = false
        output[name] = false
    end

    function ar:list_file()
        if not self._list_file then
            self._list_file = {}
            local len = #dir:string()
            for _, name in ipairs {'map', 'resource', 'scripts', 'sound', 'trigger', 'w3x2lni'} do
                scan_dir(dir / name, function (path)
                    local name = path:string():sub(len+2):lower()
                    self._list_file[#self._list_file+1] = unify(name)
                end)
            end
        end
        return self._list_file
    end
    
    function ar:number_of_files()
        return #self:list_file()
    end

    function ar:get_type()
        return 'dir'
    end

    function ar:__pairs()
        local tbl = {}
        for k, v in pairs(output) do
            if v then
                tbl[k] = v
            end
        end
        return next, tbl
    end

    return ar
end

local dummy_map
return function (mappath)
    dummy_map = nil
    storm.set_dummy_map(nil)
    local path = fs.path(mappath)
    if not is_lni(path) then
        return
    end

    log.info('Open Lni map', path)
    local dir = path:parent_path()
    dummy_map = dummy_map_ar(dir)
    storm.set_dummy_map(dummy_map)
    
    local w2l = w3x2lni()
    w2l.input_mode = 'lni'
    w2l:set_setting { mode = 'obj' }

    w2l.input_ar = dummy_map
    w2l.output_ar = dummy_map
    w2l:frontend()
    w2l:backend()
    w2l:save()
    log.info('Converted to Obj map')

    local mpq_path = tga_mpq(dummy_map)
    storm.open(mpq_path, 16)

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
            return buf
        end
    end)
end
