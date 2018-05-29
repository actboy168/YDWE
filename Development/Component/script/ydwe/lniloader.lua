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

local function dummy_map_ar(dir)
    local files = {}
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
        log.info('dummy map set', name)
        files[name] = buf
    end

    function ar:remove(name)
        log.info('dummy map remove', name)
        files[name] = false
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
        for k, v in pairs(files) do
            if v then
                tbl[k] = v
            end
        end
        return next, tbl
    end

    return ar
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
        w2l:save()
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
