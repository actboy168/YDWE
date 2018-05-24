local lang = require 'lang'

local type = type
local pairs = pairs
local os_clock = os.clock

local w2l
local revert_list
local unit_list

local function get_revert_list(default, code)
    if not revert_list then
        revert_list = {}
        for name, obj in pairs(default) do
            local code = obj['_code']
            local list = revert_list[code]
            if not list then
                revert_list[code] = name
            else
                if type(list) ~= 'table' then
                    revert_list[code] = {[list] = true}
                end
                revert_list[code][name] = true
            end
        end
    end
    return revert_list[code]
end

local function get_unit_list(default, name)
    if not unit_list then
        unit_list = {}
        for name, obj in pairs(default) do
            local _name = obj['_name']
            if _name then
                local list = unit_list[_name]
                if not list then
                    unit_list[_name] = name
                else
                    if type(list) ~= 'table' then
                        unit_list[_name] = {[list] = true}
                    end
                    unit_list[_name][name] = true
                end
            end
        end
    end
    return unit_list[name]
end

local function find_para(name, obj, default, type)
    if obj._parent and default[obj._parent] then
        return obj._parent
    end
    if default[name] and default[name]._code == obj._code then
        return name
    end
    local code = obj['_code']
    if code then
        local list = get_revert_list(default, code)
        if list then
            return list
        end
    end
    if type == 'unit' then
        local list = get_unit_list(default, obj['_name'])
        if list then
            return list
        end
    end
    return default
end

local function try_obj(obj, may_obj)
    local diff_count = 0
    for name, may_data in pairs(may_obj) do
        if name:sub(1, 1) ~= '_' then
            local data = obj[name]
            if type(may_data) == 'table' then
                if type(data) == 'table' then
                    for i = 1, #may_data do
                        if data[i] ~= may_data[i] then
                            diff_count = diff_count + 1
                            break
                        end
                    end
                else
                    diff_count = diff_count + 1
                end
            else
                if data ~= may_data then
                    diff_count = diff_count + 1
                end
            end
        end
    end
    return diff_count
end

local function parse_obj(name, obj, default, setting, ttype)
    local parent
    local count
    local find_times = setting.find_id_times or 0
    local maybe = find_para(name, obj, default, ttype)
    if type(maybe) == 'table' then
        for try_name in pairs(maybe) do
            local new_count = try_obj(obj, default[try_name])
            if not count or count > new_count or (count == new_count and parent > try_name) then
                count = new_count
                parent = try_name
            end
            find_times = find_times - 1
            if find_times == 0 then
                break
            end
        end
    else
        parent = maybe
    end

    obj._parent = parent
end

local function processing(type, chunk)
    if not chunk then
        return
    end
    local default = w2l:get_default()[type]
    local setting = w2l.setting
    local names = {}
    for name in pairs(chunk) do
        names[#names+1] = name
    end
    table.sort(names)

    revert_list = nil
    unit_list = nil
    w2l.progress(0.1)
    
    w2l.progress:start(1)
    local clock = os_clock()
    for i, name in ipairs(names) do
        parse_obj(name, chunk[name], default, setting, type)
        if os_clock() - clock >= 0.1 then
            clock = os_clock()
            w2l.messager.text(lang.script.SEARCH_TEMPLATE:format(chunk[name]._id, i, #names))
            w2l.progress(i / #names)
        end
    end
    w2l.progress:finish()
end

return function (w2l_, slk)
    w2l = w2l_
    local count = 0
    for type, name in pairs(w2l.info.obj) do
        count = count + 1
        w2l.progress:start(count / 8)
        processing(type, slk[type])
        w2l.progress:finish()
    end
end
