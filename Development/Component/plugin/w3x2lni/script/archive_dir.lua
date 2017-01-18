local sleep = require 'ffi.sleep'

local function task(f, ...)
    for i = 1, 99 do
        if pcall(f, ...) then
            return
        end
        sleep(10)
    end
    f(...)
end

local mt = {}
mt.__index = mt

function mt:save()
    if fs.exists(self.path) then
        task(fs.remove_all, self.path)
    end
    task(fs.create_directories, self.path)
    return true
end

function mt:close()
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
    return io.load(self.path / name)
end

function mt:save_file(name, buf, filetime)
    fs.create_directories((self.path / name):remove_filename())
    io.save(self.path / name, buf)
    return true
end

return function (input)
    return setmetatable({ path = input }, mt)
end
