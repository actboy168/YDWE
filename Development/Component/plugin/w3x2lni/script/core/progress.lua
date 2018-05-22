local mt = {}

mt.__index = mt

mt.level = 0
mt.current = 0
mt.min_rate = 0
mt.max_rate = 1
mt.progress = 0
mt.messager = nil

function mt:send_progress()
    local newprogress = self.current * (self.max_rate - self.min_rate) + self.min_rate
    if self.progress + 0.01 < newprogress or (newprogress == 1) then
        if self.messager then
            self.messager.progress(newprogress)
        end
        self.progress = newprogress
    end
end

function mt:refresh_rate()
    self.min_rate = 0
    self.max_rate = 1
    for i = self.level, 1, -1 do
        self.min_rate = self.min_rate * (self.max[i] - self.min[i]) + self.min[i]
        self.max_rate = self.max_rate * (self.max[i] - self.min[i]) + self.min[i]
    end
end

-- 开启新任务,新任务完成时当前任务的完成进度
function mt:start(n)
    self.level = self.level + 1
    self.min[self.level] = self.current
    self.max[self.level] = n
    self.current = 0
    self:refresh_rate()
end

-- 完成当前任务
function mt:finish()
    self.current = self.max[self.level]
    self.level = self.level - 1
    self:refresh_rate()
    self:send_progress()
end

-- 设置当前任务进度
function mt:__call(n)
    self.current = n
    self:send_progress()
end

function mt:set_messager(messager)
    self.messager = messager
end

return function ()
    local self = setmetatable({}, mt)
    self.min = {}
    self.max = {}
    return self
end
