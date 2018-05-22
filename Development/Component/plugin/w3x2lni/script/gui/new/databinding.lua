local event_queue = nil

local function event_init()
    if event_queue then
        event_queue.level = event_queue.level + 1
        return
    end
    event_queue = {
        front = 1,
        back = 1,
        level = 1,
    }
end

local function event_disable(v)
    event_queue[v] = true
end

local function event_push(v)
    if event_queue[v] then
        return
    end
    event_queue[v] = true
    event_queue[event_queue.back] = v
    event_queue.back = event_queue.back + 1
end

local function event_pop(v)
    if event_queue.front == event_queue.back then
        return
    end
    local v = event_queue[event_queue.front]
    event_queue.front = event_queue.front + 1
    return v
end

local function event_close()
    if event_queue.level > 1 then
        event_queue.level = event_queue.level - 1
        return
    end
    while true do
        local ev = event_pop()
        if not ev then
            break
        end
        ev()
    end
    event_queue = nil
end

local function getpath(root, k)
    if root then
        return root .. '.' .. k
    end
    return k
end

local function split(str)
    local r = {}
    str:gsub('[^%.]+', function (w) r[#r+1] = w end)
    return r
end

local function create_proxy(data, event, path)
    local node = {}
    for k, v in pairs(data) do
        if type(v) == 'table' then
            if not event[k] then
                event[k] = {}
            end
            node[k] = create_proxy(v, event[k], getpath(path, k))
        end
    end
    local mt = {}
    function mt:__index(k)
        if data[k] == nil then
            error(('Get `%s` is a invalid value.'):format(getpath(path, k)))
        end
        if type(data[k]) == 'table' then
            return node[k]
        end
        return data[k]
    end
    function mt:__newindex(k, v)
        if data[k] == nil then
            error(('Set `%s` is a invalid value.'):format(getpath(path, k)))
        end
        if type(data[k]) == 'table' then
            error(('Set `%s` is a table.'):format(getpath(path, k)))
        end
        data[k] = v
        event_init()
        if event[k] then
            for _, e in ipairs(event[k]) do
                event_push(e)
            end
        end
        event_close()
    end
    return setmetatable({}, mt)
end

local mt = {}
mt.__index = mt

function mt:bind(str, f)
    local event = self.event
    local data = self.data
    local path = split(str)
    local k = path[#path]
    path[#path] = nil
    for _, k in ipairs(path) do
        event = event[k]
        data = data[k]
    end
    if not event[k] then
        event[k] = {}
    end
    event = event[k]
    if f and not event[f] then
        event[f] = true
        event[#event+1] = f
    end
    return {
        get = function()
            return data[k]
        end,
        set = function(_, v)
            event_init()
            event_disable(f)
            data[k] = v
            for _, e in ipairs(event) do
                event_push(e)
            end
            event_close()
        end,
    }
end

function mt:execute(str, data_self)
    local env = setmetatable({self = data_self}, {__index = self.proxy})
    return assert(load(str, '=(databinding)', 't', env))()
end

return function (data)
    local event = {}
    local proxy = create_proxy(data, event)
    return setmetatable({ 
        proxy = proxy,
        event = event,
        data = data
     }, mt)
end
