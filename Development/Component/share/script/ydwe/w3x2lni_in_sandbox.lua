local loader = {}

local sandbox = require 'sandbox'
local w2l = sandbox('w3x2lni', {
    ['w3xparser'] = require 'w3xparser',
    ['lni-c']     = require 'lni-c',
    ['lpeg']      = require 'lpeg',
    ['loader']    = loader,
    ['io']        = { open = function(filename) return io.open(fs.path(filename)) end },
})

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
    if key == 'mpq_load' or key == 'map_load' or key == 'map_save' or key == 'map_remove' then
        loader[key] = value
    end
end

return mt
