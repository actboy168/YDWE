local loader = {}

local sandbox = require 'sandbox'
local w2l = sandbox('w3x2lni', {
    ['w3xparser'] = require 'w3xparser',
    ['lni-c']     = require 'lni-c',
    ['lpeg']      = require 'lpeg',
    ['loader']    = loader,
    ['io']        = { open = function(filename) return io.open(fs.path(filename)) end },
})

function loader:mpq_load(filename)
    if self.on_mpq_load then
        return self:on_mpq_load(filename)
    end
    return nil
end

function loader:map_load(filename)
    if self.on_map_load then
        return self:on_map_load(filename)
    end
    return nil
end

function loader:map_save(filename, buf)
    if self.on_map_save then
        return self:on_map_save(filename, buf)
    end
    return false
end

function loader:map_remove(filename)
    if self.on_map_remove then
        return self:on_map_remove(filename)
    end
    return false
end

local mt = {}
setmetatable(mt, mt)

function mt:__index(key)
    local value = w2l[key]
    if type(value) == 'function' then
        return function (obj, ...)
            if obj == self then
                obj = w2l
            end
            return value(obj, ...)
        end
    end
    return value
end

function mt:__newindex(key, value)
    if key == 'on_mpq_load' or key == 'on_map_load' or key == 'on_map_save' or key == 'on_map_remove' then
        loader[key] = value
    else
        w2l[key] = value
    end
end

return mt
