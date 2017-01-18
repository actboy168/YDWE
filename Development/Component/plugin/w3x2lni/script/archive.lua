local mpq = require 'archive_mpq'
local dir = require 'archive_dir'
local progress = require 'progress'

local os_clock = os.clock

local mt = {}
mt.__index = mt

function mt:number_of_files()
    if self:get_type() == 'mpq' then
        return self.handle:number_of_files()
    else
        return -1
    end
end

function mt:get_type()
    return self._type
end

function mt:is_readonly()
    return self._read
end

function mt:save_file(name, buf, filetime)
    return self.handle:save_file(name, buf, filetime)
end

function mt:close()
    if self._attach then
        return false
    end
    return self.handle:close()
end

function mt:save(w3i, encrypt)
    if self:is_readonly() then
        return false
    end
    if not self.handle:save(self.path, w3i, self.write_count, encrypt) then
        return false
    end
    local clock = os_clock()
    local count = 0
    for name, buf in pairs(self.write_cache) do
        if buf then
            self.handle:save_file(name, buf)
            count = count + 1
            if os_clock() - clock > 0.1 then
                clock = os_clock()
                progress(count / self.write_count)
                if self:get_type() == 'mpq' then
                    message(('正在打包文件... (%d/%d)'):format(count, self.write_count))
                else
                    message(('正在导出文件... (%d/%d)'):format(count, self.write_count))
                end
            end
        end
    end
    return true
end

function mt:set(name, buf)
    name = name:lower()
    if self.write_cache[name] then
        self.write_count = self.write_count - 1
    end
    if buf then
        self.write_count = self.write_count + 1
    end
    self.write_cache[name] = buf

    if self.handle:has_file(name) then
        if self.read_cache[name] ~= nil then
            self.read_count = self.read_count - 1
        end
        if buf ~= nil then
            self.read_count = self.read_count + 1
        end
    end
    self.read_cache[name] = buf
end

function mt:get(name)
    name = name:lower()
    if self.read_cache[name] then
        return self.read_cache[name]
    end
    if self.read_cache[name] == false then
        return nil
    end
    if not self.handle then
        return nil
    end
    local buf = self.handle:load_file(name)
    if buf then
        self.read_cache[name] = buf
        self.read_count = self.read_count + 1
    end
    return buf
end

function mt:__pairs()
    if self:is_readonly() then
        return next, self.read_cache
    else
        return next, self.write_cache
    end
end

return function (pathorhandle, tp)
    local read_only = tp ~= 'w'
    local ar = {
        read_cache = {},
        write_cache = {},
        read_count = 0,
        write_count = 0,
        path = pathorhandle,
        _read = read_only,
    }
    if type(pathorhandle) == 'number' then
        ar.handle = mpq(pathorhandle)
        ar._type = 'mpq'
        ar._attach = true
        ar._read = false
    elseif read_only then
        if fs.is_directory(pathorhandle) then
            ar.handle = dir(pathorhandle)
            ar._type = 'dir'
        else
            ar.handle = mpq(pathorhandle, true)
            ar._type = 'mpq'
        end
        if not ar.handle then
            message('地图打开失败')
            return nil
        end
    else
        if fs.is_directory(pathorhandle) then
            ar.handle = dir(pathorhandle)
            ar._type = 'dir'
        else
            ar.handle = mpq(pathorhandle)
            ar._type = 'mpq'
        end
    end
    return setmetatable(ar, mt)
end
