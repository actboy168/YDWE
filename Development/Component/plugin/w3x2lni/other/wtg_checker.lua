local w2l
local wtg
local state
local index
local read_eca
local _, a, b, c

local type_map = {
    [0] = 'event',
    [1] = 'condition',
    [2] = 'action',
    [3] = 'call',
}

local arg_count
local function get_ui_arg_count(type, name)
    if not arg_count[name] then
        local ui = state.ui[type][name]
        local count = 0
        if ui.args then
            for _, arg in ipairs(ui.args) do
                if arg.type ~= 'nothing' then
                    count = count + 1
                end
            end
        end
        arg_count[name] = count
    end
    return arg_count[name]
end

local function read_head()
    _, _, index = ('c4l'):unpack(wtg, index)
end

local function read_category()
    _, _, _, index = ('lzl'):unpack(wtg, index)
end

local function read_categories()
    a, index = ('l'):unpack(wtg, index)
    for i = 1, a do
        read_category()
    end
end

local function read_var()
    _, _, _, _, _, _, _, index = ('zzllllz'):unpack(wtg, index)
end

local function read_vars()
    _, a, index = ('ll'):unpack(wtg, index)
    for i = 1, a do
        read_var()
    end
end

local function read_arg()
    _, _, a, index = ('lzl'):unpack(wtg, index)

    if a == 1 then
        read_eca(false)
    end

    b, index = ('l'):unpack(wtg, index)
    if b == 1 then
        read_arg()
    end
    return arg
end

function read_eca(is_child)
    a, index = ('l'):unpack(wtg, index)
    if is_child then
        _, index = ('l'):unpack(wtg, index)
    end
    b, _, index = ('zl'):unpack(wtg, index)

    local count = get_ui_arg_count(type_map[a], b)
    for i = 1, count do
        read_arg()
    end
    c, index = ('l'):unpack(wtg, index)
    return c
end

local function read_ecas(count, is_child)
    for i = 1, count do
        local child_count = read_eca(is_child)
        read_ecas(child_count, true)
    end
end

local function read_trigger()
    _, _, _, _, _, _, _, _, a, index = ('zzlllllll'):unpack(wtg, index)
    read_ecas(a, false)
end

local function read_triggers()
    a, index = ('l'):unpack(wtg, index)
    for i = 1, a do
        read_trigger()
    end
end

local function check()
    read_head()
    read_categories()
    read_vars()
    read_triggers()
end

return function (w2l_, wtg_, state_)
    wtg = wtg_
    state = state_
    arg_count = {}
    index = 1

    return pcall(check)
end
