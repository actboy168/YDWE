local lang = require 'lang'
local lines
local jass
local report
local messager

local current_function
local get_exp
local add_lines

local function insert_line(str)
    lines[#lines+1] = str
end

local function get_integer(exp)
    local int = exp.value
    if int >= 1000000 then
        int = ('$%X'):format(int)
    else
        int = ('%d'):format(int)
    end
    return int
end

local function get_real(exp)
    local neg, real = exp.value:match '%s-(%-?)%s-(%S+)'
    real = real:gsub('^[0]+', ''):gsub('[0]+$', '')
    if real == '.' then
        real = '0.'
    end
    return neg .. real
end

local function get_string(exp)
    return ('"%s"'):format(exp.value:gsub('\r\n', '\\n'):gsub('[\r\n]', '\\n'))
end

local function get_boolean(exp)
    if exp.value == true then
        return 'true'
    elseif exp.value == false then
        return 'false'
    end
end

local function get_confused_name(obj)
    return obj.confused or obj.name
end

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

local function get_var_name(exp)
    local var = get_var(exp.name)
    return get_confused_name(var)
end

local function get_vari_name(exp)
    return ('%s[%s]'):format(get_var_name(exp), get_exp(exp[1]))
end

local function get_function_name(call)
    local func = get_function(call.name)
    return get_confused_name(func)
end

local function get_call(exp)
    local args = {}
    for i, sub_exp in ipairs(exp) do
        args[i] = get_exp(sub_exp)
    end
    return ('%s(%s)'):format(get_function_name(exp), table.concat(args, ','))
end

local function get_add(exp)
    return ('%s+%s'):format(get_exp(exp[1], '+', 1), get_exp(exp[2], '+', 2))
end

local function get_sub(exp)
    return ('%s-%s'):format(get_exp(exp[1], '-', 1), get_exp(exp[2], '-', 2))
end

local function get_mul(exp)
    return ('%s*%s'):format(get_exp(exp[1], '*', 1), get_exp(exp[2], '*', 2))
end

local function get_div(exp)
    return ('%s/%s'):format(get_exp(exp[1], '/', 1), get_exp(exp[2], '/', 2))
end

local function get_neg(exp)
    return ('-%s'):format(get_exp(exp[1], 'neg', 1))
end

local level = {}
level['or']    = 1
level['and']   = 2
level['>']     = 3
level['>=']    = 3
level['<']     = 3
level['<=']    = 3
level['==']    = 3
level['!=']    = 3
level['+']     = 4
level['-']     = 4
level['*']     = 5
level['/']     = 5
level['not']   = 6
level['neg']   = 6
level['paren'] = 6
local function get_equal(exp)
    return ('%s==%s'):format(get_exp(exp[1], '==', 1), get_exp(exp[2], '==', 2))
end

local function get_unequal(exp)
    return ('%s!=%s'):format(get_exp(exp[1], '!=', 1), get_exp(exp[2], '!=', 2))
end

local function get_gt(exp)
    return ('%s>%s'):format(get_exp(exp[1], '>', 1), get_exp(exp[2], '>', 2))
end

local function get_ge(exp)
    return ('%s>=%s'):format(get_exp(exp[1], '>=', 1), get_exp(exp[2], '>=', 2))
end

local function get_lt(exp)
    return ('%s<%s'):format(get_exp(exp[1], '<', 1), get_exp(exp[2], '<', 2))
end

local function get_le(exp)
    return ('%s<=%s'):format(get_exp(exp[1], '<=', 1), get_exp(exp[2], '<=', 2))
end

local function get_and(exp)
    return ('%s and %s'):format(get_exp(exp[1], 'and', 1), get_exp(exp[2], 'and', 2))
end

local function get_or(exp)
    return ('%s or %s'):format(get_exp(exp[1], 'or', 1), get_exp(exp[2], 'or', 2))
end

local function get_not(exp)
    return ('not %s'):format(get_exp(exp[1], 'not', 1))
end

local function get_code(exp)
    return ('function %s'):format(get_function_name(exp))
end

local priority = {
{'and'},
{'or'},
{'<', '>', '==', '!=', '<=', '>='},
{'not'},
{'+', '-'},
{'*', '/'},
{'neg'},
}

local op_level
local function get_op_level(op)
    if not op_level then
        op_level = {}
        for lv, ops in ipairs(priority) do
            for _, op in ipairs(ops) do
                op_level[op] = lv
            end
        end
    end
    return op_level[op]
end

local function need_paren(op1, op2, pos)
    if not op2 then
        return false
    end
    local lv1, lv2 = get_op_level(op1), get_op_level(op2)
    if not lv1 then
        return false
    end
    if pos == 1 then
        return lv1 < lv2
    elseif pos == 2 then
        return lv1 <= lv2
    end
end

function get_exp(exp, op, pos)
    if not exp then
        return nil
    end
    local value
    if exp.type == 'null' then
        value = 'null'
    elseif exp.type == 'integer' then
        value = get_integer(exp)
    elseif exp.type == 'real' then
        value = get_real(exp)
    elseif exp.type == 'string' then
        value = get_string(exp)
    elseif exp.type == 'boolean' then
        value = get_boolean(exp)
    elseif exp.type == 'var' then
        value = get_var_name(exp)
    elseif exp.type == 'vari' then
        value = get_vari_name(exp)
    elseif exp.type == 'call' then
        value = get_call(exp)
    elseif exp.type == '+' then
        value = get_add(exp)
    elseif exp.type == '-' then
        value = get_sub(exp)
    elseif exp.type == '*' then
        value = get_mul(exp)
    elseif exp.type == '/' then
        value = get_div(exp)
    elseif exp.type == 'neg' then
        value = get_neg(exp)
    elseif exp.type == '==' then
        value = get_equal(exp)
    elseif exp.type == '!=' then
        value = get_unequal(exp)
    elseif exp.type == '>' then
        value = get_gt(exp)
    elseif exp.type == '<' then
        value = get_lt(exp)
    elseif exp.type == '>=' then
        value = get_ge(exp)
    elseif exp.type == '<=' then
        value = get_le(exp)
    elseif exp.type == 'and' then
        value = get_and(exp)
    elseif exp.type == 'or' then
        value = get_or(exp)
    elseif exp.type == 'not' then
        value = get_not(exp)
    elseif exp.type == 'code' then
        value = get_code(exp)
    end
    if value then
        if need_paren(exp.type, op, pos) then
            value = ('(%s)'):format(value)
        end
        return value
    end
    messager(lang.report.UNEXPECT_EXP, exp.type)
    return nil
end

local function base_type(type)
    while jass.types[type].extends do
        type = jass.types[type].extends
    end
    return type
end

local function add_global(global)
    if not global.used then
        report(lang.report.UNREFERENCE_GLOBAL, global.name, lang.report.JASS_LINE:format(global.line))
        return
    end
    current_line = global.line
    if global.array then
        insert_line(([[%s array %s]]):format(global.type, get_confused_name(global)))
    else
        local value = get_exp(global[1])
        if value then
            insert_line(([[%s %s=%s]]):format(global.type, get_confused_name(global), value))
        else
            insert_line(([[%s %s]]):format(global.type, get_confused_name(global)))
        end
    end
end

local function add_globals()
    insert_line('globals')
    for _, global in ipairs(jass.globals) do
        add_global(global)
    end
    insert_line('endglobals')
end

local function add_local(loc)
    if not loc.used then
        report(lang.report.UNREFERENCE_LOCAL, current_function.name, (lang.report.JASS_LINE .. '[%s]'):format(loc.line, loc.name))
        return
    end
    current_line = loc.line
    if loc.array then
        insert_line(('local %s array %s'):format(loc.type, get_confused_name(loc)))
    else
        local value = get_exp(loc[1])
        if value then
            insert_line(('local %s %s=%s'):format(loc.type, get_confused_name(loc), value))
        else
            insert_line(('local %s %s'):format(loc.type, get_confused_name(loc)))
        end
    end
end

local function add_locals(locals)
    if #locals == 0 then
        return
    end
    for _, loc in ipairs(locals) do
        add_local(loc)
    end
end

local function get_args(line)
    local args = {}
    for i, exp in ipairs(line) do
        args[i] = get_exp(exp)
    end
    return table.concat(args, ',')
end

local function add_executefunc(line)
    local exp = line[1]
    if exp.type == 'string' then
        local func = get_function(line[1].value)
        if not func then
            return false
        end
        insert_line(('call ExecuteFunc("%s")'):format(get_confused_name(func)))
        return true
    end
    if not jass.confused_head then
        return false
    end
    if exp.type ~= '+' or exp[1].type ~= 'string' then
        return false
    end
    local confused = jass.confused_head[exp[1].value]
    if not confused then
        return false
    end
    insert_line(('call ExecuteFunc("%s"+%s)'):format(confused, get_exp(exp[2])))
    return true
end

local function add_call(line)
    local name = get_function_name(line)
    if name == 'ExecuteFunc' and add_executefunc(line) then
        return
    end
    insert_line(('call %s(%s)'):format(name, get_args(line)))
end

local function add_set(line)
    insert_line(('set %s=%s'):format(get_var_name(line), get_exp(line[1])))
end

local function add_seti(line)
    insert_line(('set %s[%s]=%s'):format(get_var_name(line), get_exp(line[1]), get_exp(line[2])))
end

local function add_return(line)
    if line[1] then
        insert_line(('return %s'):format(get_exp(line[1])))
    else
        insert_line('return')
    end
end

local function add_exit(line)
    insert_line(('exitwhen %s'):format(get_exp(line[1])))
end

local function add_if(data)
    insert_line(('if %s then'):format(get_exp(data.condition)))
    add_lines(data)
end

local function add_elseif(data)
    insert_line(('elseif %s then'):format(get_exp(data.condition)))
    add_lines(data)
end

local function add_else(data)
    insert_line('else')
    add_lines(data)
end

local function add_ifs(chunk)
    for _, data in ipairs(chunk) do
        if data.type == 'if' then
            add_if(data)
        elseif data.type == 'elseif' then
            add_elseif(data)
        elseif data.type == 'else' then
            add_else(data)
        else
            messager(lang.report.UNEXPECT_LOGIC, line.type)
        end
    end
    insert_line('endif')
end

local function add_loop(chunk)
    insert_line('loop')
    add_lines(chunk)
    insert_line('endloop')
end

function add_lines(chunk)
    for i, line in ipairs(chunk) do
        current_line = line.line
        if line.type == 'call' then
            add_call(line)
        elseif line.type == 'set' then
            add_set(line)
        elseif line.type == 'seti' then
            add_seti(line)
        elseif line.type == 'return' then
            add_return(line)
        elseif line.type == 'exit' then
            add_exit(line)
        elseif line.type == 'if' then
            add_ifs(line)
        elseif line.type == 'loop' then
            add_loop(line)
        else
            messager(lang.report.UNEXPECT_CHUNK, line.type)
        end
    end
end

local function get_takes(func)
    if not func.args then
        return 'nothing'
    end
    local takes = {}
    for i, arg in ipairs(func.args) do
        takes[i] = ('%s %s'):format(arg.type, get_confused_name(arg))
    end
    return table.concat(takes, ',')
end

local function get_returns(func)
    if func.returns then
        return func.returns
    else
        return 'nothing'
    end
end

local function add_native(func)
    if not func.used then
        report(lang.report.UNREFERENCE_FUNCTION, func.name, lang.report.JASS_LINE:format(func.line))
        return
    end
    current_function = func
    insert_line(([[native %s takes %s returns %s]]):format(get_confused_name(func), get_takes(func), get_returns(func)))
end

local function add_function(func)
    if not func.used then
        report(lang.report.UNREFERENCE_FUNCTION, func.name, lang.report.JASS_LINES:format(func.line, func.endline))
        return
    end
    current_function = func
    current_line = func.line
    insert_line(([[function %s takes %s returns %s]]):format(get_confused_name(func), get_takes(func), get_returns(func)))
    add_locals(func.locals)
    add_lines(func)
    insert_line('endfunction')
end

local function add_functions()
    for _, func in ipairs(jass.functions) do
        if func.native then
            add_native(func)
        else
            add_function(func)
        end
    end
end

return function (ast, _report, _messager)
    lines = {}
    jass = ast
    report = _report
    messager = _messager
    
    add_globals()
    add_functions()

    lines[#lines+1] = ''

    return table.concat(lines, '\r\n')
end
