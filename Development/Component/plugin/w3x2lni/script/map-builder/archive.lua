local mpq = require 'map-builder.archive_mpq'
local dir = require 'map-builder.archive_dir'
local search = require 'map-builder.search'
local lang = require 'share.lang'

local os_clock = os.clock

local mt = {}
mt.__index = mt

function mt:number_of_files()
    if self:get_type() == 'mpq' then
        return self.handle:number_of_files()
    else
        return self.handle:count_files()
    end
end

function mt:search_files()
    return self.handle:search_files()
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

function mt:save(w3i, w2l)
    if self:is_readonly() then
        return false
    end
    local progress = w2l.progress
    local max = 0
    for _ in pairs(self) do
        max = max + 1
    end
    local suc, res = self.handle:save(self.path, w3i, max, w2l.config.remove_we_only)
    if not suc then
        return false, res
    end
    local clock = os_clock()
    local count = 0
    for name, buf in pairs(self) do
        self.handle:save_file(name, buf)
        count = count + 1
        if os_clock() - clock > 0.1 then
            clock = os_clock()
            progress(count / max)
            if self:get_type() == 'mpq' then
                w2l.messager.text(lang.script.PACK_MAP:format(count, max))
            else
                w2l.messager.text(lang.script.EXPORT_FILE:format(count, max))
            end
        end
    end
    return true
end

function mt:flush()
    if self:is_readonly() then
        return false
    end
    self._flushed = true
    self.write_cache = {}
    self.read_cache = {}
end

local function unify(name)
    return name:lower():gsub('/', '\\'):gsub('\\[\\]+', '\\')
end

function mt:has(name)
    name = unify(name)
    if not self.handle then
        return false
    end
    if self.read_cache[name] == false then
        return false
    end
    if self.read_cache[name] then
        return true
    end
    if self._flushed then
        return
    end
    local buf = self.handle:load_file(name)
    if buf then
        self.read_cache[name] = buf
        return true
    else
        self.read_cache[name] = false
        return false
    end
end

function mt:set(name, buf)
    name = unify(name)
    self.write_cache[name] = buf
end

function mt:remove(name)
    name = unify(name)
    self.write_cache[name] = false
end

function mt:get(name)
    name = unify(name)
    if self.write_cache[name] then
        return self.write_cache[name]
    end
    if self.write_cache[name] == false then
        return nil
    end
    if not self.handle then
        return nil
    end
    if self._flushed then
        return nil
    end
    if self.read_cache[name] == false then
        return nil
    end
    if self.read_cache[name] then
        return self.read_cache[name]
    end
    local buf = self.handle:load_file(name)
    if buf then
        self.read_cache[name] = buf
    else
        self.read_cache[name] = false
    end
    return buf
end

function mt:__pairs()
    local tbl = {}
    for k, v in pairs(self.write_cache) do
        if v then
            tbl[k] = v
        end
    end
    return next, tbl
end

function mt:search_files(progress)
    if not self._searched then
        self._searched = true
        search(self, progress)
    end
    local files = {}
    for name, buf in pairs(self.read_cache) do
        if buf and self.write_cache[name] == nil then
            files[name] = buf
        end
    end
    return next, files
end

return function (pathorhandle, tp)
    if not pathorhandle then
        return nil
    end
    local read_only = tp ~= 'w'
    local err
    local ar = {
        write_cache = {},
        read_cache = {},
        path = pathorhandle,
        _read = read_only,
    }
    if type(pathorhandle) == 'number' then
        ar.handle, err = mpq(pathorhandle)
        ar._type = 'mpq'
        ar._attach = true
        ar._read = false
    else
        if tp ~= 'w' and not fs.exists(pathorhandle) then
            if fs.exists(pathorhandle:parent_path() / (pathorhandle:filename():string() .. '.w3x')) then
                return nil, lang.script.OPEN_FAILED_MAYBE_W3X:format(pathorhandle:filename())
            else
                return nil, lang.script.OPEN_FAILED_NO_EXISTS:format()
            end
        end
        if read_only then
            if fs.is_directory(pathorhandle) then
                ar.handle, err = dir(pathorhandle)
                ar._type = 'dir'
            else
                ar.handle, err = mpq(pathorhandle, true)
                ar._type = 'mpq'
            end
            if not ar.handle then
                return nil, err or lang.script.OPEN_FAILED
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
    end
    return setmetatable(ar, mt)
end
