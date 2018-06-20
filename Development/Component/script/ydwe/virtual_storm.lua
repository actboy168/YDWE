local storm = require 'ffi.storm'

local api = {}
local dummy_map

function api.open(path, priority)
    return storm.open(path, priority)
end

function api.close(handle)
    return storm.close(handle)
end

function api.extract_file(path, name)
    if dummy_map then
        local buf = dummy_map:get(name)
        if buf then
            io.save(path, buf)
            return
        end
    end
    return storm.extract_file(path, name)
end

function api.load_file(name)
    if dummy_map then
        local buf = dummy_map:get(name)
        if buf then
            return buf
        end
    end
    return storm.load_file(name)
end

function api.has_file(name)
    if dummy_map then
        local buf = dummy_map:get(name)
        if buf then
            return true
        end
    end
    return storm.has_file(name)
end

function api.set_dummy_map(map)
    dummy_map = map
end

function api.get_dummy_map()
    return dummy_map
end

return api
