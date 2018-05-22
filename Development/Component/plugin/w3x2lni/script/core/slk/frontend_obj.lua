local select = select
local string_lower = string.lower
local string_unpack = string.unpack
local string_match = string.match

local w2l
local wts
local default
local has_level
local unpack_buf
local unpack_pos

local function set_pos(...)
    unpack_pos = select(-1, ...)
    return ...
end

local function unpack(str)
    return set_pos(string_unpack(str, unpack_buf, unpack_pos))
end

local function read_data(obj)
    local data = {}
    local id = string_match(unpack 'c4', '^[^\0]+')
    local value_type = unpack 'l'
    local level = 0

    --是否包含等级信息
    if has_level then
        local this_level = unpack 'l'
        level = this_level
        -- 扔掉一个整数
        unpack 'l'
    end

    local value
    if value_type == 0 then
        value = unpack 'l'
    elseif value_type == 1 or value_type == 2 then
        value = unpack 'f'
    else
        local str = unpack 'z'
        value = w2l:load_wts(wts, str)
    end
    
    -- 扔掉一个整数
    unpack 'l'
    
    if level == 0 then
        level = 1
    end
    if not obj[id] then
        obj[id] = {}
    end
    obj[id][level] = value
end

local function read_obj(chunk, type)
    local parent, name = unpack 'c4c4'
    if name == '\0\0\0\0' then
        name = parent
    end
    local obj = {
        _id = name,
        _parent = parent,
        _type = type,
        _obj = true,
    }

    local count = unpack 'l'
    for i = 1, count do
        read_data(obj)
    end
    if not default or not default[parent] then
        w2l.force_slk = true
    end
    chunk[name] = obj
    return obj
end

local function read_version()
    return unpack 'l'
end

local function read_chunk(chunk, type)
    local count = unpack 'l'
    for i = 1, count do
        local obj = read_obj(chunk, type)
    end
end

return function (w2l_, type, buf, wts_)
    w2l = w2l_
    if type == 'misc' then
        return w2l:frontend_misc(buf)
    end
    wts = wts_
    default = w2l:get_default()[type]
    has_level = w2l.info.key.max_level[type]
    unpack_buf = buf
    unpack_pos = 1
    local data    = {}
    -- 版本号
    read_version()
    -- 默认数据
    read_chunk(data, type)
    -- 自定义数据
    read_chunk(data, type)
    return data
end
