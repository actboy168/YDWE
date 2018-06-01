local grammar = require 'parser.grammar'
local lang = require 'lang'

local jass
local ast
local messager

local parse_exp
local parse_lines

local function parser_error(str)
    local line = ast.current_line
    local start = 1
    while true do
        start = jass:find('[\r\n]', start)
        if not start then
            start = 1
            break
        end
        if jass:sub(start, start + 1) == '\r\n' then
            start = start + 2
        else
            start = start + 1
        end
        line = line - 1
        if line <= 1 then
            break
        end
    end
    local finish = jass:find('%f[\r\n]', start) or #jass
    error(lang.parser.ERROR_POS:format(str, ast.file, ast.current_line, jass:sub(start, finish)))
end

local function base_type(type)
    while ast.types[type].extends do
        type = ast.types[type].extends
    end
    return type
end

local function get_var(name)
    if ast.current_function then
        if ast.current_function.locals[name] then
            return ast.current_function.locals[name]
        end
        if ast.current_function.args and ast.current_function.args[name] then
            return ast.current_function.args[name]
        end
    end
    local var = ast.globals[name]
    if not var then
        parser_error(lang.parser.VAR_NO_EXISTS:format(name))
    end
    return var
end

local function get_function(name)
    return ast.functions[name]
end

local function get_var_type(exp)
    local var = get_var(exp.name)
    return var.type
end

local function get_vari_type(exp)
    local var = get_var(exp.name)
    parse_exp(exp[1], 'integer')
    return var.type
end

local function get_call(exp)
    local func = ast.functions[exp.name]
    if not func then
        parser_error(lang.parser.FUNCTION_NO_EXISTS:format(exp.name))
    end
    for _, arg in ipairs(exp) do
        parse_exp(arg)
    end
    return func.returns or 'null'
end

local function get_op(exp)
    local t1 = parse_exp(exp[1])
    local t2 = parse_exp(exp[2])
    if (t1 == 'integer' or t1 == 'real') and (t2 == 'integer' or t2 == 'real') then
        if t1 == 'real' or t2 == 'real' then
            return 'real'
        else
            return 'integer'
        end
    end
    return nil, t1, t2
end

local function get_add(exp)
    local type, t1, t2 = get_op(exp)
    if type then
        return type
    end
    if (t1 == 'string' or t1 == 'null') and (t2 == 'string' or t2 == 'null') then
        return 'string'
    end
    parser_error(lang.parser.ERROR_ADD:format(t1, t2))
end

local function get_sub(exp)
    local type, t1, t2 = get_op(exp)
    if type then
        return type
    end
    parser_error(lang.parser.ERROR_SUB:format(t1, t2))
end

local function get_mul(exp)
    local type, t1, t2 = get_op(exp)
    if type then
        return type
    end
    parser_error(lang.parser.ERROR_MUL:format(t1, t2))
end

local function get_div(exp)
    local type, t1, t2 = get_op(exp)
    if type then
        return type
    end
    parser_error(lang.parser.ERROR_DIV:format(t1, t2))
end

local function get_neg(exp)
    local t = parse_exp(exp[1])
    if t == 'real' or t == 'integer' then
        return t
    end
    parser_error(lang.parser.ERROR_NEG:format(t))
end

local function get_equal(exp)
    local t1 = parse_exp(exp[1])
    local t2 = parse_exp(exp[2])
    if t1 == 'null' or t2 == 'null' then
        return 'boolean'
    end
    if (t1 == 'integer' or t1 == 'real') and (t2 == 'integer' or t2 == 'real') then
        return 'boolean'
    end
    local b1 = base_type(t1)
    local b2 = base_type(t2)
    if b1 == b2 then
        return 'boolean'
    end
    parser_error(lang.parser.ERROR_EQUAL:format(t1, t2))
end

local function get_compare(exp)
    local t1 = parse_exp(exp[1])
    local t2 = parse_exp(exp[2])
    if (t1 == 'integer' or t1 == 'real') and (t2 == 'integer' or t2 == 'real') then
        return 'boolean'
    end
    parser_error(lang.parser.ERROR_COMPARE:format(t1, t2))
end

local function get_and(exp)
    parse_exp(exp[1], 'boolean')
    parse_exp(exp[2], 'boolean')
    return 'boolean'
end

local function get_or(exp)
    parse_exp(exp[1], 'boolean')
    parse_exp(exp[2], 'boolean')
    return 'boolean'
end

local function get_not(exp)
    parse_exp(exp[1], 'boolean')
    return 'boolean'
end

local function get_code(exp)
    return 'code'
end

function parse_exp(exp, expect)
    if exp.type == 'null' then
        exp.vtype = 'null'
    elseif exp.type == 'integer' then
        exp.vtype = 'integer'
    elseif exp.type == 'real' then
        exp.vtype = 'real'
    elseif exp.type == 'string' then
        exp.vtype = 'string'
    elseif exp.type == 'boolean' then
        exp.vtype = 'boolean'
    elseif exp.type == 'var' then
        exp.vtype = get_var_type(exp)
    elseif exp.type == 'vari' then
        exp.vtype = get_vari_type(exp)
    elseif exp.type == 'call' then
        exp.vtype = get_call(exp)
    elseif exp.type == '+' then
        exp.vtype = get_add(exp)
    elseif exp.type == '-' then
        exp.vtype = get_sub(exp)
    elseif exp.type == '*' then
        exp.vtype = get_mul(exp)
    elseif exp.type == '/' then
        exp.vtype = get_div(exp)
    elseif exp.type == 'neg' then
        exp.vtype = get_neg(exp)
    elseif exp.type == 'paren' then
        exp.vtype = parse_exp(exp[1])
    elseif exp.type == '==' then
        exp.vtype = get_equal(exp)
    elseif exp.type == '!=' then
        exp.vtype = get_equal(exp)
    elseif exp.type == '>' then
        exp.vtype = get_compare(exp)
    elseif exp.type == '<' then
        exp.vtype = get_compare(exp)
    elseif exp.type == '>=' then
        exp.vtype = get_compare(exp)
    elseif exp.type == '<=' then
        exp.vtype = get_compare(exp)
    elseif exp.type == 'and' then
        exp.vtype = get_and(exp)
    elseif exp.type == 'or' then
        exp.vtype = get_or(exp)
    elseif exp.type == 'not' then
        exp.vtype = get_not(exp)
    elseif exp.type == 'code' then
        exp.vtype = get_code(exp)
    else
        messager(lang.parser.UNKNOWN_EXP, exp.type)
    end
    if not exp.vtype then
        messager(lang.parser.UNKNOWN_EXP_TYPE, exp.type)
    end
    return exp.vtype
end

local function parse_type(data)
    ast.current_line = data.line
    if not ast.types[data.extends] then
        parser_error(lang.parser.ERROR_TYPE:format(data.extends))
    end
    if ast.types[data.name] and not ast.types[data.name].extends then
        parser_error(lang.parser.ERROR_DEFINE_NATIVE_TYPE)
    end
    if ast.types[data.name] then
        parser_error(lang.parser.ERROR_REDEFINE_TYPE:format(data.name, ast.types[data.name].file, ast.types[data.name].line))
    end
    ast.types[data.name] = data
end

local function parse_global(data)
    ast.current_line = data.line
    if ast.globals[data.name] then
        parser_error(lang.parser.ERROR_REDEFINE_GLOBAL:format(data.name, ast.globals[data.name].file, ast.globals[data.name].line))
    end
    if data.constant and not data[1] then
        parser_error(lang.parser.ERROR_CONSTANT_INIT)
    end
    if not ast.types[data.type] then
        parser_error(lang.parser.ERROR_UNDEFINE_TYPE:format(data.type))
    end
    if data.array and data[1] then
        parser_error(lang.parser.ERROR_ARRAY_INIT)
    end
    if data[1] then
        parse_exp(data[1], data.type)
    end
    table.insert(ast.globals, data)
    ast.globals[data.name] = data
end

local function parse_globals(chunk)
    for _, func in ipairs(ast.functions) do
        if not func.native then
            ast.current_line = chunk.line
            parser_error(lang.parser.ERROR_GLOBAL_AFTER_FUNCTION)
        end
    end
    for _, data in ipairs(chunk) do
        parse_global(data)
    end
end

local function parse_arg(data, args)
    args[data.name] = data
end

local function parse_args(chunk)
    if not chunk.args then
        return
    end
    for _, arg in ipairs(chunk.args) do
        parse_arg(arg, chunk.args)
    end
end

local function parse_local(data, locals, args)
    ast.current_line = data.line
    if not ast.types[data.type] then
        parser_error(lang.parser.ERROR_TYPE:format(data.type))
    end
    if data.array and data[1] then
        parser_error(lang.parser.ERROR_ARRAY_INIT)
    end
    if args and args[data.name] then
        parser_error(lang.parser.ERROR_LOCAL_NAME_WITH_ARG:format(data.name))
    end
    if data[1] then
        parse_exp(data[1], data.type)
    end
    locals[data.name] = data
end

local function parse_locals(chunk)
    for _, data in ipairs(chunk.locals) do
        parse_local(data, chunk.locals, chunk.args)
    end
end

local function parse_loop(chunk)
    ast.loop_count = ast.loop_count + 1
    parse_lines(chunk)
    ast.loop_count = ast.loop_count - 1
end

local function parse_if(data)
    for _, chunk in ipairs(data) do
        if chunk.type == 'if' or chunk.type == 'elseif' then
            parse_exp(chunk.condition, 'boolean')
        end
        parse_lines(chunk)
    end
end

local function parse_call(line)
    local func = get_function(line.name)
    if not func then
        parser_error(lang.parser.FUNCTION_NO_EXISTS:format(line.name))
    end
    if not func.args then
        return
    end
    for i, arg in ipairs(func.args) do
        parse_exp(line[i], arg.type)
    end
end

local function parse_set(line)
    local var = get_var(line.name)
    parse_exp(line[1], var.vtype)
end

local function parse_seti(line)
    local var = get_var(line.name)
    parse_exp(line[1], 'integer')
    parse_exp(line[2], var.vtype)
end

local function parse_return(line)
    local func = ast.current_function
    if not func.returns then
        return
    end
    parse_exp(line[1], func.returns)
end

local function parse_exit(line)
    if ast.loop_count == 0 then
        parser_error(lang.parser.ERROR_EXITWHEN)
    end
    parse_exp(line[1], 'boolean')
end

local function parse_line(line)
    ast.current_line = line.line
    if line.type == 'loop' then
        parse_loop(line)
    elseif line.type == 'if' then
        parse_if(line)
    elseif line.type == 'call' then
        parse_call(line)
    elseif line.type == 'set' then
        parse_set(line)
    elseif line.type == 'seti' then
        parse_seti(line)
    elseif line.type == 'return' then
        parse_return(line)
    elseif line.type == 'exit' then
        parse_exit(line)
    else
        parser_error(lang.parser.UNKNOWN_LINE..line.type)
    end
end

function parse_lines(chunk)
    for i, line in ipairs(chunk) do
        parse_line(line)
    end
end

local function parse_function(chunk)
    table.insert(ast.functions, chunk)
    ast.functions[chunk.name] = chunk
    
    if chunk.native then
        return
    end

    ast.current_function = chunk
    ast.loop_count = 0
    
    parse_args(chunk)
    parse_locals(chunk)
    parse_lines(chunk)

    ast.current_function = nil
end

local function parser_gram(gram)
    for i, chunk in ipairs(gram) do
        if chunk.type == 'globals' then
            parse_globals(chunk)
        elseif chunk.type == 'function' then
            parse_function(chunk)
        elseif chunk.type == 'type' then
            parse_type(chunk)
        else
            parser_error(lang.parser.UNKNOWN_CHUNK..chunk.type)
        end
    end
end

return function (jass_, file, _ast, _messager)
    jass = jass_
    messager = _messager or print
    if _ast then
        ast = _ast

        for i = 1, #ast.functions do
            ast.functions[i] = nil
        end
        for i = 1, #ast.globals do
            ast.globals[i] = nil
        end
    else
        ast = {}
        ast.types = {
            null    = {type = 'type'},
            handle  = {type = 'type'},
            code    = {type = 'type'},
            integer = {type = 'type'},
            real    = {type = 'type'},
            boolean = {type = 'type'},
            string  = {type = 'type'},
        }
        ast.globals = {}
        ast.functions = {}
    end

    ast.file = file

    local gram, comments = grammar(jass, file, messager)

    parser_gram(gram)

    ast.current_line = nil
    ast.loop_count = nil
    ast.file = nil
    
    ast.comments = comments
    
    return ast, gram
end
