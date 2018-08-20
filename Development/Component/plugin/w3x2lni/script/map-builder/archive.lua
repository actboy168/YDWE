local mpq = require 'map-builder.archive_mpq'
local dir = require 'map-builder.archive_dir'
local lang = require 'share.lang'

local os_clock = os.clock

local mt = {}
mt.__index = mt

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

function mt:save(w3i, w3f, w2l)
    if self:is_readonly() then
        return false
    end
    local progress = w2l.progress
    local max = 0
    for _ in pairs(self) do
        max = max + 1
    end
    local suc, res = self.handle:save(self.path, w3i, w3f, max, w2l.setting.remove_we_only)
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

function mt:list_file()
    return self.handle:list_file()
end

function mt:number_of_files()
    return self.handle:number_of_files()
end

function mt:flush()
    self.handle:flush()
end

function mt:has(name)
    if not self.handle then
        return false
    end
    local lname = name:lower()
    self.case[lname] = name
    if self.cache[lname] then
        return true
    end
    local buf = self.handle:load_file(name)
    if buf then
        self.cache[lname] = buf
        return true
    else
        self.cache[lname] = false
        return false
    end
end

function mt:set(name, buf)
    if not self.handle then
        return
    end
    local lname = name:lower()
    self.cache[lname] = buf
    self.case[lname] = name
end

function mt:remove(name)
    if not self.handle then
        return
    end
    local lname = name:lower()
    self.cache[lname] = false
    self.case[lname] = name
end

function mt:get(name)
    if not self.handle then
        return nil
    end
    local lname = name:lower()
    self.case[lname] = name
    if self.cache[lname] then
        return self.cache[lname]
    end
    if self.cache[lname] == false then
        return nil
    end
    local buf = self.handle:load_file(name)
    if buf then
        self.cache[lname] = buf
    else
        self.cache[lname] = false
    end
    return buf
end

function mt:__pairs()
    local case = self.case
    local tbl = {}
    for k, v in pairs(self.cache) do
        if v then
            tbl[case[k]] = v
        end
    end
    return next, tbl
end

return function (pathorhandle, tp)
    if not pathorhandle then
        return nil
    end
    local read_only = tp ~= 'w'
    local err
    local ar = {
        path = pathorhandle,
        cache = {},
        case = {},
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
                ar.handle, err = dir(pathorhandle, true)
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
