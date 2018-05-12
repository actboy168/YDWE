local mt = {}
mt.__index = mt

function mt:add(format, ...)
    self.hexs[#self.hexs+1] = (format):pack(...)
end

function mt:add_head()
    self:add('c4LL', 'W3do', 8, 11)
end

function mt:add_data()
    self:add('L', 0)
end

return function(self)
    local tbl = setmetatable({}, mt)
    tbl.hexs = {}
    tbl.self = self

    tbl:add_head()
    tbl:add_data()

    return table.concat(tbl.hexs)
end
