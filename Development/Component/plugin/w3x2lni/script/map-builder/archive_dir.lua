local ignore = {}
for _, name in ipairs {'.git', '.svn', '.vscode', '.gitignore'} do
    ignore[name] = true
end

local esc_map = {}
local loaded_map = {}
for c in ('/:*?"<>|$'):gmatch '.' do
    esc_map[c] = ('$%02x'):format(c:byte())
end
for i = 0, 31 do
    esc_map[string.char(i)] = ('$%02x'):format(i)
end
for k, v in pairs(esc_map) do
    loaded_map[v] = k
end
local function esced_name(name)
    name = name:gsub('[^\\]*', function (path)
        if path == '' then
            return '$'
        end
        return path:gsub('.', esc_map)
    end)
    return name
end

local function loaded_name(name)
    name = name:gsub('[^\\]*', function (path)
        if path == '$' then
            return ''
        end
        return path:gsub('$..', loaded_map)
    end)
    return name
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
        scan_dir(self.path, function (path)
            local name = path:string():sub(len+2):gsub('/', '\\')
            self._list_file[#self._list_file+1] = loaded_name(name)
        end)
    end
    return self._list_file
end

function mt:number_of_files()
    return #self:list_file()
end

function mt:extract(name, path)
    return fs.copy_file(self.path / esced_name(name), path, true)
end

function mt:has_file(name)
    return fs.exists(self.path / esced_name(name))
end

function mt:remove_file(name)
    fs.remove(self.path / esced_name(name))
end

function mt:load_file(name)
    local new_name = esced_name(name)
    local f = io.open((self.path / new_name):string(), 'rb')
    if not f then
        return nil
    end
    local buf = f:read 'a'
    f:close()
    return buf
end

function mt:save_file(name, buf, filetime)
    local new_name = esced_name(name)
    local dir = (self.path / new_name):parent_path()
    if not fs.exists(dir) then
        fs.create_directories(dir)
    end
    io.save(self.path / new_name, buf)
    return true
end

return function (input)
    return setmetatable({ path = input }, mt)
end
