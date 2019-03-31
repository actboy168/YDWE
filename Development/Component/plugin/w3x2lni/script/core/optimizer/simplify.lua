local confuser = require 'optimizer.confuser'
local lang = require 'lang'

local ipairs = ipairs
local pairs = pairs

local jass, report, confuse1, confuse2
local current_function, current_line, has_call
local executes, executed_any, global_variable_any
local mark_exp, mark_lines, mark_function

local function get_function(name)
    return jass.functions[name]
end

local function get_arg(name)
    if current_function and current_function.args then
        return current_function.args[name]
    end
end

local function get_local(name)
    if current_function then
        local locals = current_function.locals
        if locals then
            for i = #locals, 1, -1 do
                local loc = locals[i]
                if loc.name == name and loc.line < current_line then
                    return loc
                end
            end
        end
    end
end

local function get_global(name)
    return jass.globals[name]
end

local function get_var(name)
    local var = get_local(name)
    if var then
        return var, 'local'
    end
    local var = get_arg(name)
    if var then
        return var, 'arg'
    end
    local var = get_global(name)
    if var then
        return var, 'global'
    end
end

local function mark_var(var)
    local use_var, type = get_var(var.name)
    if type == 'global' and use_var.file ~= 'war3map.j' then
        return
    end
    use_var.used = true
    if confuse1 then
        use_var.confused = confuse1(var.name)
    end
end

function mark_exp(exp)
    if not exp then
        return
    end
    if exp.type == 'null' or exp.type == 'integer' or exp.type == 'real' or exp.type == 'string' or exp.type == 'boolean' then
    elseif exp.type == 'var' or exp.type == 'vari' then
        mark_var(exp)
    elseif exp.type == 'call' then
        has_call = true
        mark_function(exp)
    elseif exp.type == 'code' then
        mark_function(exp)
    end
    for i = 1, #exp do
        mark_exp(exp[i])
    end
end

local function mark_locals(locals)
    for _, loc in ipairs(locals) do
        if loc[1] then
            current_line = loc.line
            has_call = false
            mark_exp(loc[1])
            if has_call then
                loc.used = true
                if confuse1 then
                    loc.confused = confuse1(loc.name)
                end
            end
        end
    end
end

local function mark_execute(line)
    if not executes then
        executes = {}
    end
    local exp = line[1]
    if exp.type == 'string' then
        if get_function(exp.value) then
            mark_function(get_function(exp.value))
        end
        return
    end
    if exp.type == '+' then
        if exp[1].type == 'string' then
            local head = exp[1].value
            executes[head] = true
            report(lang.report.REFERENCE_FUNCTION, ('%s...'):format(head), (lang.report.JASS_LINE .. 'ExecuteFunc("%s" + ...)'):format(line.line, head))
            return
        end
    end
    if not executed_any then
        executed_any = true
        report(lang.report.CONFUSE_JASS, lang.report.FORCE_REFERENCE_ALL, (lang.report.JASS_LINE .. lang.report.FULLY_DYNAMIC .. 'ExecuteFunc'):format(line.line))
        if confuse1 then
            report(lang.report.CONFUSE_JASS, lang.report.NO_CONFUSING_FUNCTION_NAME, (lang.report.JASS_LINE .. lang.report.FULLY_DYNAMIC .. 'ExecuteFunc'):format(line.line))
        end
    end
end

local function check_confuse(line)
    if confuse1 then
        if not global_variable_any then
            global_variable_any = true
            report(lang.report.CONFUSE_JASS, lang.report.NO_CONFUSING_GLOBAL_REAL, (lang.report.JASS_LINE .. lang.report.DEFINED_VARIABLE_EVENT):format(line.line))
        end
    end
end

local function mark_call(line)
    mark_function(line)
    for _, exp in ipairs(line) do
        mark_exp(exp)
    end
    if line.name == 'ExecuteFunc' then
        mark_execute(line)
    end
    if line.name == 'TriggerRegisterVariableEvent' then
        check_confuse(line)
    end
end

local function mark_set(line)
    mark_var(line)
    mark_exp(line[1])
end

local function mark_seti(line)
    mark_var(line)
    mark_exp(line[1])
    mark_exp(line[2])
end

local function mark_return(line)
    if line[1] then
        mark_exp(line[1])
    end
end

local function mark_exit(line)
    mark_exp(line[1])
end

local function mark_if(data)
    mark_exp(data.condition)
    mark_lines(data)
end

local function mark_elseif(data)
    mark_exp(data.condition)
    mark_lines(data)
end

local function mark_else(data)
    mark_lines(data)
end

local function mark_ifs(chunk)
    for _, data in ipairs(chunk) do
        if data.type == 'if' then
            mark_if(data)
        elseif data.type == 'elseif' then
            mark_elseif(data)
        else
            mark_else(data)
        end
    end
end

local function mark_loop(chunk)
    mark_lines(chunk)
end

function mark_lines(lines)
    for _, line in ipairs(lines) do
        current_line = line.line
        if line.type == 'call' then
            mark_call(line)
        elseif line.type == 'set' then
            mark_set(line)
        elseif line.type == 'seti' then
            mark_seti(line)
        elseif line.type == 'return' then
            mark_return(line)
        elseif line.type == 'exit' then
            mark_exit(line)
        elseif line.type == 'if' then
            mark_ifs(line)
        elseif line.type == 'loop' then
            mark_loop(line)
        end
    end
end

local function mark_takes(args)
    if not args then
        return
    end
    for _, arg in ipairs(args) do
        if confuse1 then
            arg.confused = confuse1(arg.name)
        end
    end
end

function mark_function(call)
    if not call then
        return
    end
    local func = get_function(call.name)
    if func.native then
        func.used = true
        return
    end
    if func.used or func.file ~= 'war3map.j' then
        return
    end
    func.used = true
    local _current_function = current_function
    local _current_line     = current_line
    current_function = func
    mark_takes(func.args)
    mark_locals(func.locals)
    mark_lines(func)
    current_function = _current_function
    current_line     = _current_line
end

local function mark_globals()
    for _, global in ipairs(jass.globals) do
        if global[1] then
            current_line = global.line
            has_call = false
            mark_exp(global[1])
            if has_call then
                global.used = true
                if confuse1 then
                    global.confused = confuse1(global.name)
                end
            end
        end
    end
end

local function fix_globals()
    if not global_variable_any then
        return
    end
    for _, global in ipairs(jass.globals) do
        if global.type == 'real' and not global.array then
            global.confused = nil
        end
    end
end

local function mark_executed_used(func)
    if func.used then
        return
    end
    if executed_any then
        mark_function(func)
        return
    end
    local name = func.name
    for head in pairs(executes) do
        if name:sub(1, #head) == head then
            mark_function(func)
            return
        end
    end
end

local function mark_executed_confuse(func)
    local name = func.name
    if name == 'config' or name == 'main' then
        return
    end
    if func.native then
        return
    end
    for head in pairs(executes) do
        if name:sub(1, #head) == head then
            func.confused = confuse2(head) .. name:sub(#head+1)
            jass.confused_head[head] = confuse2(head)
            return
        end
    end
    func.confused = confuse1(name)
end

local function mark_executed()
    if not executes then
        return
    end
    for _, func in ipairs(jass.functions) do
        mark_executed_used(func)
    end
    if not executed_any and confuse1 then
        for _, func in ipairs(jass.functions) do
            mark_executed_confuse(func)
        end
    end
end

local cant_use = {'globals', 'endglobals', 'constant', 'native', 'array', 'and', 'or', 'not', 'type', 'extends', 'function', 'endfunction', 'nothing', 'takes', 'returns', 'call', 'set', 'return', 'if', 'endif', 'elseif', 'else', 'loop', 'endloop', 'exitwhen', 'main', 'config'}
for _, name in ipairs(cant_use) do
    cant_use[name] = true
end

local function can_use(name)
    if cant_use[name] then
        return false
    end
    local func = get_function(name)
    if func then
        if func.file ~= 'war3map.j' then
            return false
        end
        return true
    end
    local var, type = get_var(name)
    if type == 'global' then
        if var.file ~= 'war3map.j' then
            return false
        end
        return true
    elseif type == 'arg' or type == 'local' then
        return true
    end
    return true
end

local function init_confuser(confused, confusion)
    if not confused then
        return
    end

    confusion = tostring(confusion)
    local chars = {}
    for char in confusion:gmatch '[%w_]' do
        if not chars[char] then
            chars[#chars+1] = char
        end
    end
    if #chars < 3 then
        report(lang.report.CONFUSE_JASS, lang.report.CONFUSED_FAILED, lang.report.NEED_3_CHARS)
    end
    
    confusion = table.concat(chars)

    local count = 0
    for _ in confusion:gmatch '%a' do
        count = count + 1
    end
    if count < 2 then
        report(lang.report.CONFUSE_JASS, lang.report.CONFUSED_FAILED, lang.report.NEED_2_LETTERS)
        return
    end

    local confuse_head = confusion:match '%a'
    confuse1 = confuser(confusion:gsub(confuse_head, ''))
    function confuse1:on_find(name)
        if can_use(name) then
            return name
        else
            return nil
        end
    end

    jass.confused_head = {}
    confuse2 = confuser(confusion)
    function confuse2:on_find(name)
        name = confuse_head .. name
        if can_use(name) then
            return name
        else
            return nil
        end
    end
end

return function (ast, setting, _report)
    jass = ast
    report = _report

    init_confuser(setting.confused, setting.confusion)
    mark_globals()
    mark_function(get_function 'config')
    mark_function(get_function 'main')
    mark_executed()
    fix_globals()
end
