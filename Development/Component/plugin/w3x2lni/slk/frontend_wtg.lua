local w2l
local wtg
local state
local chunk
local unpack_index
local read_eca
local fix
local retry_point
local need_retry

local fix_step
local trigger_types
local preset_map
local fixed_preset
local var_map
local unknowtypes
local unknowindex

local function assert_then_retry(b, info)
    if b then
        return
    end
    need_retry = true
    error(info)
end

local function fix_arg(n)
    n = n or #fix_step
    if n <= 0 then
        if #fix_step > 0 then
            error '未知UI参数超过100个，放弃修复。'
        else
            error '触发器文件错误。'
        end
    end
    local step = fix_step[n]
    if #step.args >= 100 then
        step.args = {}
        w2l.message(('猜测[%s]的参数数量为[0]'):format(step.name))
        return fix_arg(n-1)
    end
    table.insert(step.args, {})
    w2l.message(('猜测[%s]的参数数量为[%d]'):format(step.name, #step.args))
    return step.save_point
end

local function try_fix(tp, name)
    if not fix.ui[tp] then
        fix.ui[tp] = {}
    end
    if not fix.ui[tp][name] then
        w2l.message(('触发器UI[%s]不存在'):format(name))
        fix.ui[tp][name] = {
            name = name,
            fix = true,
            args = {},
            category = 'TC_UNKNOWUI',
            save_point = save_point,
        }
        table.insert(fix_step, fix.ui[tp][name])
        
        if not fix.categories[tp]['TC_UNKNOWUI'] then
            fix.categories[tp]['TC_UNKNOWUI'] = {}
            table.insert(fix.categories[tp], 'TC_UNKNOWUI')
        end
        table.insert(fix.categories[tp]['TC_UNKNOWUI'], fix.ui[tp][name])
        w2l.message(('猜测[%s]的参数数量为[0]'):format(name))
        try_count = 0
    end
    return fix.ui[tp][name]
end

local function new_unknow()
    local name
    if not trigger_types then
        trigger_types = {}
        for _, typedef in ipairs(state.ui.define.TriggerTypes) do
            trigger_types[typedef[1]] = true
        end
    end
    repeat 
        unknowindex = unknowindex + 1
        name = ('未知类型%03d'):format(unknowindex)
    until not trigger_types[name]
    table.insert(unknowtypes, name)
    return name
end

local type_map = {
    [0] = 'event',
    [1] = 'condition',
    [2] = 'action',
    [3] = 'call',
}

local function get_ui_define(type, name)
    return state.ui[type][name] or try_fix(type, name)
end

local function unpack(fmt)
    local result
    result, unpack_index = fmt:unpack(wtg, unpack_index)
    return result
end

local function read_head()
    chunk.file_id  = unpack 'c4'
    chunk.file_ver = unpack 'l'
end

local function read_category()
    local category = {}
    category.id      = unpack 'l'
    category.name    = unpack 'z'
    category.comment = unpack 'l'
    return category
end

local function read_categories()
    local count = unpack 'l'
    chunk.categories = {}
    for i = 1, count do
        table.insert(chunk.categories, read_category())
    end
end

local function read_var()
    local var = {}
    var.name         = unpack 'z'
    var.type         = unpack 'z'
    var.int_unknow_1 = unpack 'l'
    var.is_arry      = unpack 'l'
    var.array_size   = unpack 'l'
    var.is_default   = unpack 'l'
    var.value        = unpack 'z'
    return var
end

local function read_vars()
    chunk.int_unknow_1 = unpack 'l'
    local count = unpack 'l'
    chunk.vars = {}
    for i = 1, count do
        table.insert(chunk.vars, read_var())
    end
end

local arg_type_map = {
    [-1] = 'disabled',
    [0]  = 'preset',
    [1]  = 'var',
    [2]  = 'call',
    [3]  = 'constant',
}

local function get_valid(name, ui_type)
    if ui_type == 'boolean' then
        return false
    elseif ui_type == 'integer' then
        return 0
    elseif ui_type == 'real' then
        return 0.0
    else
        return name
    end
end

local function get_preset_type(name, ui_type, ui_guess_level)
    if not preset_map then
        preset_map = {}
        for _, line in ipairs(state.ui.define.TriggerParams) do
            local key = line[1]
            local type = line[2]:match '%d+%,([%w_]+)%,'
            if type == 'typename' then
                preset_map[key] = {key:match '^.-_(.+)$', 1.0}
            else
                preset_map[key] = {type, 1.0}
            end
        end
    end
    if preset_map[name] then
        return preset_map[name][1], preset_map[name][2]
    elseif fixed_preset[name] then
        if ui_guess_level > fixed_preset[name][2] then
            fixed_preset[name][1] = ui_type
            fixed_preset[name][2] = ui_guess_level
        end
        return fixed_preset[name][1], fixed_preset[name][2]
    else
        fixed_preset[name] = {ui_type, ui_guess_level}
        return fixed_preset[name][1], fixed_preset[name][2]
    end
end

local function get_var_type(name)
    if not var_map then
        var_map = {}
        for _, var in ipairs(chunk.vars) do
            var_map[var.name] = var.type
        end
    end
    return var_map[name], 1.0
end

local function get_ui_returns(ui, ui_type, ui_guess_level)
    if not ui.fix then
        if ui.returns == 'AnyReturnType' then
            return ui_type, ui_guess_level
        else
            return ui.returns, 1.0
        end
    end
    if not ui.returns_guess_level then
        ui.returns = ui_type
        ui.returns_guess_level = ui_guess_level
    end
    if ui.returns ~= ui_type and ui.returns_guess_level < ui_guess_level then
        ui.returns = ui_type
        ui.returns_guess_level = ui_guess_level
        retry_point = ui.save_point
        error(('重新计算[%s]的参数类型。'):format(ui.name))
    end
    return ui.returns, ui.returns_guess_level
end

local function get_call_type(name, eca_type, ui_type, ui_guess_level)
    local ui = get_ui_define(eca_type, name)
    if ui then
        return get_ui_returns(ui, ui_type, ui_guess_level)
    else
        return ui_type, ui_guess_level
    end
end

local function get_constant_type(value)
    if value == 'true' or value == 'false' then
        return 'boolean', 0.1
    elseif value:match '^[%-]?[1-9][%d]*$' then
        return 'integer', 0.2
    elseif value:match '^[%-]?[1-9][%d]*[%.][%d]*$' then
        return 'real', 0.3
    elseif #value > 128 then
        return 'StringExt', 1.0
    else
        return 'string', 0.4
    end
end

local function get_arg_type(arg, ui_type, ui_guess_level)
    local atp = arg_type_map[arg.type]
    if atp == 'disabled' then
        return new_unknow(), 0.0
    elseif atp == 'preset' then
        return get_preset_type(arg.value, ui_type, ui_guess_level)
    elseif atp == 'var' then
        return get_var_type(arg.value)
    elseif atp == 'call' then
        return get_call_type(arg.eca.name, type_map[arg.eca.type], ui_type, ui_guess_level)
    else
        return get_constant_type(arg.value)
    end
end

local function fix_arg_type(ui, ui_arg, arg, args)
    local ui_arg_type
    local ui_guess_level
    if ui.fix then
        if not ui_arg.type then
            ui_arg.type = new_unknow()
            ui_arg.guess_level = 0.0
        end
        ui_arg_type = ui_arg.type
        ui_guess_level = ui_arg.guess_level
    else
        ui_arg_type = ui_arg.type
        ui_guess_level = 1.0
    end
    if ui_arg_type == 'AnyGlobal' then
        ui_guess_level = 0.0
    elseif ui_arg_type == 'Null' then
        local ok
        local start
        for i = #args, 1, -1 do
            if start then
                if ui.args[i].type == 'AnyGlobal' or ui.args[i].type == 'typename' then
                    ui_arg_type = args[i].arg_type
                    ui_guess_level = args[i].arg_guess_level
                    ok = true
                    break
                end
            elseif args[i] == arg then
                start = true
            end
        end
        assert(ok)
    end
    local tp, arg_guess_level = get_arg_type(arg, ui_arg_type, ui_guess_level)
    if ui.fix and arg_guess_level > ui_guess_level and ui_arg_type ~= 'AnyGlobal' and ui_arg_type ~= 'Null' then
        ui_arg.type = tp
        ui_arg.guess_level = arg_guess_level
    end
    arg.arg_type = tp
    arg.arg_guess_level = arg_guess_level
end

local function read_arg()
    local arg = {}
    arg.type        = unpack 'l'
    arg.value       = unpack 'z'
    arg.insert_call = unpack 'l'
    assert_then_retry(arg_type_map[arg.type], 'arg.type 错误')
    assert_then_retry(arg.insert_call == 0 or arg.insert_call == 1, 'arg.insert_call 错误')

    if arg.insert_call == 1 then
        local eca_type = unpack 'l'
        arg.eca = read_eca(false, eca_type)
    end

    arg.insert_index = unpack 'l'
    assert_then_retry(arg.insert_index == 0 or arg.insert_index == 1, 'arg.insert_index 错误')
    if arg.insert_index == 1 then
        arg.index = read_arg()
    end
    return arg
end

function read_eca(is_child, eca_type)
    local eca = {}
    if eca_type then
        eca.type = eca_type
    else
        eca.type = unpack 'l'
        if is_child then
            eca.child_id = unpack 'l'
        end
    end
    eca.name   = unpack 'z'
    eca.enable = unpack 'l'

    assert_then_retry(type_map[eca.type], 'eca.type 错误')
    assert_then_retry(eca.name:match '^[%g%s]*$', ('eca.name 错误：[%s]'):format(eca.name))
    assert_then_retry(eca.enable == 0 or eca.enable == 1, 'eca.enable 错误')

    eca.args = {}
    local ui = get_ui_define(type_map[eca.type], eca.name)
    if ui.args then
        for _, arg in ipairs(ui.args) do
            if arg.type ~= 'nothing' then
                local arg = read_arg(ui)
                table.insert(eca.args, arg)
                fix_arg_type(ui, ui.args[#eca.args], arg, eca.args)
            end
        end
    end
    eca.child_count = unpack 'l'
    return eca
end

local function read_ecas(ecas, count, is_child)
    for i = 1, count do
        local eca = read_eca(is_child)
        table.insert(ecas, eca)
        read_ecas(ecas, eca.child_count, true)
    end
end

local function read_trigger()
    local trigger = {}
    trigger.name     = unpack 'z'
    trigger.des      = unpack 'z'
    trigger.type     = unpack 'l'
    trigger.enable   = unpack 'l'
    trigger.wct      = unpack 'l'
    trigger.init     = unpack 'l'
    trigger.run_init = unpack 'l'
    trigger.category = unpack 'l'

    assert_then_retry(trigger.type == 0 or trigger.type == 1, 'trigger.type 错误')
    assert_then_retry(trigger.enable == 0 or trigger.enable == 1, 'trigger.enable 错误')
    assert_then_retry(trigger.wct == 0 or trigger.wct == 1, 'trigger.wct 错误')
    assert_then_retry(trigger.init == 0 or trigger.init == 1, 'trigger.init 错误')
    assert_then_retry(trigger.run_init == 0 or trigger.run_init == 1, 'trigger.run_init 错误')

    trigger.ecas = {}
    local count = unpack 'l'
    read_ecas(trigger.ecas, count, false)

    return trigger
end

local function read_triggers()
    local count = unpack 'l'
    chunk.triggers = {}
    local pos = 1
    while true do
        local suc, err = pcall(function()
            for i = pos, count do
                save_point = { i, unpack_index }
                chunk.triggers[i] = read_trigger()
            end
        end)
        if suc then
            break
        else
            try_count = try_count + 1
            if not need_retry then
                error(err)
            end
            need_retry = false
            if try_count > 1000 then
                error('在大量尝试后放弃修复。')
            end
            w2l.message(err)
            if retry_point then
                pos, unpack_index = retry_point[1], retry_point[2]
                retry_point = false
            else
                local load_point = fix_arg()
                pos, unpack_index = load_point[1], load_point[2]
            end
        end
    end
end

local function fill_fix()
    for _, type in pairs(type_map) do
        for _, ui in pairs(fix.ui[type]) do
            local arg_types = {}
            local comment = {}
            local comment_guess = {}
            local comment_unknow = {}
            for i, arg in ipairs(ui.args) do
                if not arg.type then
                    arg.type = new_unknow()
                    arg.guess_level = 0
                end
                table.insert(arg_types, (' ${%s} '):format(arg.type))
                if arg.guess_level == 0 then
                    table.insert(comment_unknow, i)
                elseif arg.guess_level < 1 then
                    table.insert(comment_guess, i)
                end
            end
            if #comment_unknow > 0 then
                table.insert(comment, ('第 %s 个参数未知。'):format(table.concat(comment_unknow, ',')))
            end
            if #comment_guess > 0 then
                table.insert(comment, ('第 %s 个参数不确定。'):format(table.concat(comment_guess, ',')))
            end
            if ui.returns then
                if ui.returns_guess_level == 0 then
                    table.insert(comment, ('返回类型未知。'):format(i))
                elseif ui.returns_guess_level < 1 then
                    table.insert(comment, ('返回类型不确定。'):format(i))
                end
            end
            ui.title = ('%s'):format(ui.name)
            ui.description = ('%s(%s)'):format(ui.name, table.concat(arg_types, ','))
            ui.comment = table.concat(comment)
        end
    end
    
    table.insert(fix.ui.define.TriggerCategories, { 'TC_UNKNOWUI', '[未知UI],ReplaceableTextures\\CommandButtons\\BTNPolymorph.blp' })
    for _, name in ipairs(unknowtypes) do
        table.insert(fix.ui.define.TriggerTypes, { name, ('0,0,0,%s,string'):format(name)})
    end

    for name, value in pairs(fixed_preset) do
        table.insert(fix.ui.define.TriggerParams, {
            name,
            ('0,%s,%s,%s'):format(value[1], get_valid(name, value[1]), name),
        })
    end
    table.sort(fix.ui.define.TriggerParams, function(a, b)
        return a[1] < b[1]
    end)
end

return function (w2l_, wtg_, state_)
    w2l = w2l_
    wtg = wtg_
    state = state_
    unpack_index = 1
    try_count = 0
    fix = {
        ui = {
            action = {},
            call = {},
            condition = {},
            event = {},
            define = {
                TriggerParams = {},
                TriggerCategories = {},
                TriggerTypes = {},
            },
        },
        categories = {
            action = {},
            define = {},
            call = {},
            condition = {},
            event = {},
        },
    }
    fix_step = {}
    chunk = {}

    trigger_types = nil
    preset_map = nil
    fixed_preset = {}
    var_map = nil
    unknowtypes = {}
    unknowindex = 0

    read_head()
    read_categories()
    read_vars()
    read_triggers()
    
    fill_fix()
    
    return chunk, fix
end
