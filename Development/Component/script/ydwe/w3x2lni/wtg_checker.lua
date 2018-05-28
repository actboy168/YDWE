local wtg
local ui
local index
local read_eca
local _, a, b, c

local ipairs = ipairs
local string_unpack = string.unpack

local type_map = {
    [0] = 'event',
    [1] = 'condition',
    [2] = 'action',
    [3] = 'call',
}

local arg_count
local function get_ui_arg_count(type, name)
    local count = arg_count[name]
    if not count then
        local ui = ui[type][name]
        count = 0
        if ui.args then
            for _, arg in ipairs(ui.args) do
                if arg.type ~= 'nothing' then
                    count = count + 1
                end
            end
        end
        arg_count[name] = count
    end
    return count
end

local function read_head()
    a, b, index = string_unpack('c4l', wtg, index)
    if a == 'WTG!' and b == 7 then
        return true
    else
        return false
    end
end

local function read_category()
    _, _, _, index = string_unpack('lzl', wtg, index)
end

local function read_categories()
    a, index = string_unpack('l', wtg, index)
    for i = 1, a do
        read_category()
    end
end

local function read_var()
    _, _, _, _, _, _, _, index = string_unpack('zzllllz', wtg, index)
end

local function read_vars()
    _, a, index = string_unpack('ll', wtg, index)
    for i = 1, a do
        read_var()
    end
end

local function read_arg()
    _, a, b, index = string_unpack('zll', wtg, index + 4)

    if a == 1 then
        read_eca(false, b)
        b, index = string_unpack('l', wtg, index + 4)
    end

    if b == 1 then
        read_arg()
    end
    return arg
end

function read_eca(is_child, a)
    if a then
        b, _, index = string_unpack('zl', wtg, index)
    else
        if is_child then
            a, _, b, _, index = string_unpack('llzl', wtg, index)
        else
            a, b, _, index = string_unpack('lzl', wtg, index)
        end
    end

    local count = get_ui_arg_count(type_map[a], b)
    for i = 1, count do
        read_arg()
    end
end

local function read_ecas(is_child)
    a, index = string_unpack('l', wtg, index)
    for i = 1, a do
        read_eca(is_child)
        read_ecas(true)
    end
end

local function read_trigger()
    _, _, index = string_unpack('zz', wtg, index)
    index = index + 24
    read_ecas(false)
end

local function read_triggers()
    a, index = string_unpack('l', wtg, index)
    for i = 1, a do
        read_trigger()
    end
end

local function check()
    local suc = read_head()
    if not suc then
        return
    end
    read_categories()
    read_vars()
    read_triggers()
end

return function (wtg_, state_)
    wtg = wtg_
    ui = state_.ui
    arg_count = {}
    index = 1

    return pcall(check)
end
