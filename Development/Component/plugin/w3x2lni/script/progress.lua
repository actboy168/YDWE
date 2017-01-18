local mt = {}
setmetatable(mt, mt)

local level = 0
local current = 0
local min_rate = 0
local max_rate = 1
local min = {}
local max = {}
local progress = 0

local function send_progress()
    local newprogress = current * (max_rate - min_rate) + min_rate
    if progress + 0.01 < newprogress then
        message('-progress', newprogress)
        progress = newprogress
    end
end

local function refresh_rate()
    min_rate = 0
    max_rate = 1
    for i = level, 1, -1 do
        min_rate = min_rate * (max[i] - min[i]) + min[i]
        max_rate = max_rate * (max[i] - min[i]) + min[i]
    end
end

-- 开启新任务,新任务完成时当前任务的完成进度
function mt:start(n)
    level = level + 1
    min[level] = current
    max[level] = n
    current = 0
    refresh_rate()
end

-- 完成当前任务
function mt:finish()
    current = max[level]
    level = level - 1
    refresh_rate()
    send_progress()
end

-- 设置当前任务进度
function mt:__call(n)
    current = n
    send_progress()
end

return mt
