local ignore = {}
for _, name in ipairs {'.git', '.svn', '.vscode', '.gitignore'} do
    ignore[name] = true
end

local function unify(name)
    return name:lower():gsub('/', '\\'):gsub('\\[\\]+', '\\')
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

local mt = {}
mt.__index = mt

function mt:save(path, dirs)
    return true
end

function mt:close()
end

function mt:list_file()
    if not self._list_file then
        self._list_file = {}
        local len = #self.path:string()
        for _, name in ipairs {'map', 'resource', 'scripts', 'sound', 'trigger', 'w3x2lni'} do
            scan_dir(self.path / name, function (path)
                local name = path:string():sub(len+2):lower()
                self._list_file[#self._list_file+1] = unify(name)
            end)
        end
    end
    return self._list_file
end

function mt:number_of_files()
    return #self:list_file()
end

function mt:extract(name, path)
    return fs.copy_file(self.path / name, path, true)
end

function mt:has_file(name)
    return fs.exists(self.path / name)
end

function mt:remove_file(name)
    fs.remove(self.path / name)
end

function mt:load_file(name)
    local f = io.open((self.path / name):string(), 'rb')
    if not f then
        return nil
    end
    local buf = f:read 'a'
    f:close()
    return buf
end

function mt:save_file(name, buf, filetime)
    local dir = (self.path / name):parent_path()
    if not fs.exists(dir) then
        fs.create_directories(dir)
    end
    io.save(self.path / name, buf)
    return true
end

return function (input)
    return setmetatable({ path = input }, mt)
end
