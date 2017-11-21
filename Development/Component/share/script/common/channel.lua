local thd = require 'thread'

local mt = {}
mt.__index = mt

function mt:send(...)
    thd.send(self.id, ...)
end

function mt:recv()
    return thd.recv(self.id)
end

function mt:close()
    thd.close(self.id)
end

return function(c)
    return setmetatable(c or {id = thd.channel()}, mt)
end
