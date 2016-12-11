require 'luabind'
require 'filesystem'
local uni = require 'unicode'

local mt = {}
mt.__index = mt

function mt:lock()
	if fs.exists(self.filename) then
		error('lock in ' .. self.filename:string())
	end
    local f, e = io.open(uni.u2a(self.filename:string()), 'w')
    if not f then
        error(e)
    end
    f:close()
end

function mt:unlock()
    fs.remove_all(self.filename)
end

return function(filename)
    return setmetatable({ filename = fs.path(filename) }, mt)
end
