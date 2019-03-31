local grammar = require 'parser.grammar'
local calcline = require 'parser.calcline'
local lang = require 'lang'

local tonumber = tonumber
local tointeger = math.tointeger
local stringByte = string.byte
local stringUnpack = string.unpack
local ipairs = ipairs
local paris = pairs

local jass
local state
local file
local linecount
local option
local ast
local errors
local Extends, Type, Integers, Code, Var

local function pushErrors(str, level, type)
    local err = {
        jass = jass,
        file = file,
        line = linecount,
        pos = 0,
        err = str,
        level = level,
        type = type,
        code = calcline.line(jass, linecount),
    }
    errors[#errors+1] = err
    return err
end

local function parserError(str)
    pushErrors(str, 'error')
end

local function parserWarning(str, type)
    pushErrors(str, 'warning', type)
end

local function parserRB(str)
    state.lastrb = pushErrors(str, 'error', 'rb')
end

local function fixRB()
    if state.lastrb then
        state.lastrb.level = 'warning'
    end
end

local function finishRB()
    state.lastrb = nil
end

local reserved = {}
for _, key in ipairs {'globals', 'endglobals', 'constant', 'native', 'array', 'and', 'or', 'not', 'type', 'extends', 'function', 'endfunction', 'nothing', 'takes', 'returns', 'call', 'set', 'return', 'if', 'then', 'endif', 'elseif', 'else', 'loop', 'endloop', 'exitwhen', 'local', 'true', 'false'} do
    reserved[key] = true
end

local bad_natives_in_globals = {
    -- 在全局变量定义中，以下API会返回null
    ['OrderId']        = 'null',
    ['OrderId2String'] = 'null',
    ['UnitId2String']  = 'null',
    -- 在全局变量定义中，以下API会使魔兽崩溃
    ['GetObjectName']     = 'crash',
    ['CreateQuest']       = 'crash',
    ['CreateMultiboard']  = 'crash',
    ['CreateLeaderboard'] = 'crash',
}

local function validName(name)
    if reserved[name] then
        parserError(lang.parser.ERROR_KEY_WORD:format(name))
        return
    end
end

local function newNameCheckTypes(name)
    local types = state.types
    if types[name] then
        if types[name].extends then
            parserError(lang.parser.ERROR_REDEFINE_TYPE:format(name, types[name].file, types[name].line))
        else
            parserError(lang.parser.ERROR_DEFINE_NATIVE_TYPE:format(name))
        end
        return
    end
end

local function newNameCheckGlobals(name)
    local globals = state.globals
    if globals[name] then
        parserError(lang.parser.ERROR_REDEFINE_GLOBAL:format(name, globals[name].file, globals[name].line))
        return
    end
end

local function newNameCheckFunctions(name)
    local functions = state.functions
    if functions[name] then
        parserError(lang.parser.ERROR_REDEFINE_FUNCTION:format(name, functions[name].file, functions[name].line))
        return
    end
end

local function newName(name)
    newNameCheckTypes(name)
    newNameCheckGlobals(name)
    newNameCheckFunctions(name)
end

local function calcExtends(a, b)
    local types = state.types
    if not types[a] or not types[b] then
        return true
    end
    if a == 'integer' and b == 'real' then
        return true
    end
    if a == 'null' then
        if b == 'code' or b == 'string' then
            return true
        end
        if b == 'handle' then
            return true
        end
        return calcExtends(b, 'handle')
    end
    while true do
        if not types[a] then
            return true
        end
        if not types[a].extends then
            break
        end
        a = types[a].extends
        if a == b then
            return true
        end
    end
    return a == b
end

local function newCache()
    local function cache(f)
        return setmetatable({}, {__index = function (self, k)
            local v = f(k)
            if k then
                self[k] = v
            end
            return v
        end})
    end

    Extends = cache(function (b)
        return cache(function (a)
            return calcExtends(a, b)
        end)
    end)

    Type = cache(function (type)
        if type == 'boolean' or type == 'integer' or type == 'real' or type == 'string' then
            return {
                type = type,
                vtype = type,
            }
        else
            return {
                type = 'null',
                vtype = 'null',
            }
        end
    end)

    Integers = cache(function (int)
        return {
            type  = 'integer',
            vtype = 'integer',
            value = int,
        }
    end)

    Code = cache(function (name)
        return {
            type = 'code',
            vtype = 'code',
            name = name,
        }
    end)

    Var = cache(function (var)
        return {
            type = 'var',
            vtype = var.type,
            name = var.name,
            _var = var,
        }
    end)
end

local function baseType(type)
    while state.types[type].extends do
        type = state.types[type].extends
    end
    return type
end

local function isExtends(a, b)
    if not a or not b then
        return true
    end
    if a == b then
        return true
    end
    return Extends[b][a]
end

local function getExploitText(var)
    local name = var.name
    if var == state.exploit[name] and not state.locals[name] then
        return lang.parser.WARNING_REDEFINE_VAR:format(name, var.file, var.line)
    end
    return ''
end

local static = {
    NULL = {
        type = 'null',
        vtype = 'null',
    },
    TRUE = {
        type  = 'boolean',
        vtype = 'boolean',
    },
    FALSE = {
        type  = 'boolean',
        vtype = 'boolean',
    },
    RETURN = {
        type = 'return',
    },
    INTEGER = {
        type = 'integer',
        vtype = 'integer',
    },
    REAL = {
        type = 'real',
        vtype = 'real',
    },
    STRING = {
        type = 'string',
        vtype = 'string',
    },
}

local function getOp(t1, t2)
    if (t1 == 'integer' or t1 == 'real') and (t2 == 'integer' or t2 == 'real') then
        if t1 == 'real' or t2 == 'real' then
            return 'real'
        else
            return 'integer'
        end
    end
    return nil
end

local function getAdd(t1, t2)
    local vtype = getOp(t1, t2)
    if vtype then
        return vtype
    end
    if (t1 == 'string' or t1 == 'null') and (t2 == 'string' or t2 == 'null') then
        return 'string'
    end
    parserError(lang.parser.ERROR_ADD:format(t1, t2))
end

local function getSub(t1, t2)
    local vtype = getOp(t1, t2)
    if vtype then
        return vtype
    end
    parserError(lang.parser.ERROR_SUB:format(t1, t2))
end

local function getMul(t1, t2)
    local vtype = getOp(t1, t2)
    if vtype then
        return vtype
    end
    parserError(lang.parser.ERROR_MUL:format(t1, t2))
end

local function getDiv(t1, t2)
    local vtype = getOp(t1, t2)
    if vtype then
        return vtype
    end
    parserError(lang.parser.ERROR_DIV:format(t1, t2))
end

local function getMod(t1, t2)
    if t1 ~= 'integer' or t2 ~= 'integer' then
        parserError(lang.parser.ERROR_MOD)
    end
    if option.Version then
        if option.Version < 29 then
            parserError(lang.parser.WARNING_MOD)
        end
    else
        parserWarning(lang.parser.WARNING_MOD)
    end
    return 'integer'
end

local function getEqual(t1, t2)
    if t1 == 'null' or t2 == 'null' then
        return 'boolean'
    end
    if (t1 == 'integer' or t1 == 'real') and (t2 == 'integer' or t2 == 'real') then
        return 'boolean'
    end
    local b1 = baseType(t1)
    local b2 = baseType(t2)
    if b1 == b2 then
        return 'boolean'
    end
    parserError(lang.parser.ERROR_EQUAL:format(t1, t2))
end

local function getCompare(t1, t2)
    if (t1 == 'integer' or t1 == 'real') and (t2 == 'integer' or t2 == 'real') then
        return 'boolean'
    end
    parserError(lang.parser.ERROR_COMPARE:format(t1, t2))
end

local function getAnd(t1, t2)
    return 'boolean'
end

local function getOr(t1, t2)
    return 'boolean'
end

local function getBinary(op, e1, e2)
    local t1 = e1.vtype
    local t2 = e2.vtype
    if not t1 or not t2 then
        return
    end
    
    if op == '+' then
        return getAdd(t1, t2)
    elseif op == '-' then
        return getSub(t1, t2)
    elseif op == '*' then
        return getMul(t1, t2)
    elseif op == '/' then
        return getDiv(t1, t2)
    elseif op == '%' then
        return getMod(t1, t2)
    elseif op == '==' or op == '!=' then
        return getEqual(t1, t2)
    elseif op == '>' or op == '<' or op == '>=' or op == '<=' then
        return getCompare(t1, t2)
    elseif op == 'and' then
        return getAnd(t1, t2)
    elseif op == 'or' then
        return getOr(t1, t2)
    end

    if op == '=' then
        parserError(lang.parser.ERROR_ASSIGN_AS_EQ)
        return getEqual(t1, t2)
    end
end

local function getUnary(op, exp)
    local t = exp.vtype
    if op == 'not' then
        return t
    end
end

local function getFunction(name)
    validName(name)
    local func = state.functions[name]
    if not func then
        parserError(lang.parser.FUNCTION_NO_EXISTS:format(name))
        return {}
    end
    return func
end

local function checkType(type)
    if type and not state.types[type] then
        parserError(lang.parser.ERROR_UNDEFINE_TYPE:format(type))
    end
end

local function checkArgs(args)
    if not args then
        return
    end
    local types = state.types
    for _, arg in ipairs(args) do
        if not types[arg.type] then
            parserError(lang.parser.ERROR_UNDEFINE_TYPE:format(arg.type))
        end
    end
end

local function checkCall(func, call)
    if not func.name then
        return
    end
    if func.args then
        local argn = #func.args
        local calln = #call
        if argn ~= calln then
            if argn > calln then
                local args = {}
                for i = calln+1, argn do
                    local arg = func.args[i]
                    args[#args+1] = arg.type .. ' ' .. arg.name
                end
                local miss = table.concat(args, ', ')
                parserError(lang.parser.ERROR_LESS_ARGS:format(func.name, argn, calln, miss))
                return
            else
                parserError(lang.parser.ERROR_MORE_ARGS:format(func.name, argn, calln))
            end
        end
        for i, arg in ipairs(func.args) do
            local exp = call[i]
            local t1, t2 = exp.vtype, arg.vtype
            if not isExtends(t1, t2) then
                local exploitText = ''
                if exp._var then
                    exploitText = getExploitText(exp._var)
                end
                parserError(lang.parser.ERROR_WRONG_ARG:format(func.name, i, t2, t1) .. exploitText)
            end
        end
        --if func.native and (func.name == 'Filter' or func.name == 'Condition') then
        --    local code = getFunction(call[1].name)
        --    if code.returns ~= 'boolean' then
        --        parserWarning(lang.parser.ERROR_FILTER_CODE)
        --    end
        --end
    else
        if #call == 0 then
            return
        end
        parserError(lang.parser.ERROR_WASTE_ARGS:format(func.name, #call))
    end
end

local function checkSet(var, source, array, exp)
    -- 如果是马甲变量，就不再检查更多错误
    if source == 'dummy' then
        return
    end
    local exploitText = getExploitText(var)
    local name = var.name
    if array then
        if not var.array then
            parserError(lang.parser.ERROR_WASTE_INDEX:format(name) .. exploitText)
        end
    else
        if var.array then
            parserError(lang.parser.ERROR_NO_INDEX:format(name) .. exploitText)
        end
    end
    if var.constant then
        parserError(lang.parser.ERROR_SET_CONSTANT:format(name))
    end
    if source == 'global' and state.currentFunction then
        if state.currentFunction.constant then
            parserError(lang.parser.ERROR_SET_IN_CONSTANT:format(name))
        end
    end
    if not isExtends(exp.vtype, var.type) then
        parserError(lang.parser.ERROR_SET_TYPE:format(name, var.type, exp.vtype) .. exploitText)
    end
end

local function checkGet(var, source, array)
    -- 如果是马甲变量，就不再检查更多错误
    if source == 'dummy' then
        return
    end

    local exploitText = getExploitText(var)
    local name = var.name
    if array then
        if not var.array then
            parserError(lang.parser.ERROR_WASTE_INDEX:format(name) .. exploitText)
        end
    else
        if var.array then
            parserError(lang.parser.ERROR_NO_INDEX:format(name) .. exploitText)
        end
        if not var._set then
            parserWarning(lang.parser.ERROR_GET_UNINIT:format(name) .. exploitText, 'runtime')
        end
    end
end

local function checkLocalWithArgs(name, type, array)
    local var = state.args[name]
    if not var then
        return
    end
    if array then
        local func = state.currentFunction
        parserError(lang.parser.ERROR_REDEFINE_ARRAY_WITH_ARG:format(name, func.name, func.file, func.line))
        return
    end
    if type ~= var.vtype then
        local func = state.currentFunction
        parserError(lang.parser.ERROR_REDEFINE_VAR_TYPE_WITH_ARG:format(name, type, func.name, var.vtype, func.file, func.line))
        return
    end
    local func = state.currentFunction
    parserWarning(lang.parser.ERROR_REDEFINE_ARG:format(name, func.file, func.line))
end

local function checkLocalWithGlobals(name, type, array)
    local var = state.globals[name]
    if not var then
        return
    end
    if array and not var.array then
        parserError(lang.parser.ERROR_REDEFINE_ARRAY_WITH_GLOBAL:format(name, var.file, var.line))
        return
    else
        parserWarning(lang.parser.ERROR_REDEFINE_GLOBAL:format(name, var.file, var.line), 'shadowing')
    end
end

local function checkArgWithGlobals(name, type)
    local var = state.globals[name]
    if not var then
        return
    end
    parserWarning(lang.parser.ERROR_REDEFINE_GLOBAL:format(name, var.file, var.line), 'shadowing')
end

local function getVar(name)
    validName(name)

    local var = state.locals[name]
    if var then
        return var, 'local'
    end

    local var = state.args[name]
    if var then
        return var, 'arg'
    end
    
    local var = state.globals[name]
    if var then
        local exploit = state.exploit[name]
        if exploit then
            return exploit, 'global'
        else
            return var, 'global'
        end
    end

    parserError(lang.parser.VAR_NO_EXISTS:format(name))
    return { name = name }, 'dummy', nil
end

local function returnOneTime()
    local stack = state.returnStack
    if stack then
        local hasReturned = state.returnMarks[stack]
        if not hasReturned then
            state.returnTimes[stack] = state.returnTimes[stack] - 1
            state.returnMarks[stack] = true
            state.returnAny = true
        end
    end
end

local parser = {}

function parser.nl()
    linecount = linecount + 1
end

function parser.File()
    return file
end

function parser.Line()
    return linecount
end

function parser.NULL()
    return static.NULL
end

function parser.TRUE()
    return static.TRUE
end

function parser.FALSE()
    return static.FALSE
end

function parser.String(str)
    return static.STRING
end

function parser.Real(str)
    return static.REAL
end

function parser.Integer8(neg, str)
    return static.INTEGER
end

function parser.Integer10(neg, str)
    return static.INTEGER
end

function parser.Integer16(neg, str)
    return static.INTEGER
end

function parser.Integer256(neg, str)
    if #str == 4 then
        if str:find('\\', 1, true) then
            parserError(lang.parser.ERROR_INT256_ESC)
        end
    end
    return static.INTEGER
end

function parser.Code(name, pl)
    local func = getFunction(name)
    if pl then
        parserError(lang.parser.ERROR_CODE_HAS_CODE:format(name))
    elseif func.args then
        parserWarning(lang.parser.ERROR_CODE_HAS_CODE:format(name), 'crash')
    end
    return Code[name]
end

function parser.ACall(name, ...)
    local func = getFunction(name)
    if state.currentFunction and state.currentFunction.constant and not func.constant then
        parserError(lang.parser.ERROR_CALL_IN_CONSTANT:format(name))
    end
    local call = {
        type = 'call',
        name = name,
        ...
    }
    checkCall(func, call)
    return call
end

function parser.Vari(name, exp)
    local var, source = getVar(name)
    checkGet(var, source, true)
    return {
        type = 'vari',
        vtype = var.type,
        name = name,
        _var = var,
    }
end

function parser.Var(name)
    local var, source = getVar(name)
    checkGet(var, source, false)
    return Var[var]
end

function parser.Neg(exp)
    local t = exp.vtype
    if t ~= 'real' and t ~= 'integer' then
        parserError(lang.parser.ERROR_NEG:format(t))
    end
    return Type[t]
end

function parser.Binary(...)
    local e1, op = ...
    if not op then
        return e1
    end
    local args = {...}
    local e1 = args[1]
    local e2
    for i = 2, #args, 2 do
        op, e2 = args[i], args[i+1]
        local vtype = getBinary(op, e1, e2)
        e1 = Type[vtype]
    end
    return e1
end

function parser.Unary(...)
    local e1, op = ...
    if not op then
        return e1
    end
    local args = {...}
    local e1 = args[#args]
    for i = #args - 1, 1, -1 do
        op = args[i]
        local vtype = getUnary(op, e1)
        e1 = Type[vtype]
    end
    return e1
end

function parser.Type(name, extends)
    local types = state.types
    checkType(extends)
    validName(name)
    newName(name)
    local type = {
        type    = 'type',
        file    = file,
        line    = linecount,
        name    = name,
        extends = extends,
    }
    types[name] = type
    return type
end

function parser.GlobalsStart()
    for _, func in ipairs(ast.functions) do
        if not func.native then
            parserError(lang.parser.ERROR_GLOBAL_AFTER_FUNCTION)
        end
    end
    state.globalsStart = linecount
end

function parser.GlobalsEnd(m)
    if m == '' then
        parserError(lang.parser.ERROR_ENDGLOBALS:format(state.globalsStart))
    end
end

function parser.Globals(globals)
    globals.type = 'globals'
    globals.file = file
    globals.line = state.globalsStart
    return globals
end

function parser.Global(constant, type, array, name, exp)
    validName(name)
    newName(name)
    checkType(type)
    local globals = state.globals
    if constant == '' then
        constant = nil
    else
        constant = true
        if not exp then
            parserError(lang.parser.ERROR_CONSTANT_INIT)
        end
    end
    if array == '' then
        array = nil
    else
        array = true
    end
    if array then
        if exp then
            parserError(lang.parser.ERROR_ARRAY_INIT)
        end
        if type == 'code' then
            parserError(lang.parser.ERROR_CODE_ARRAY)
        end
    end
    if exp then
        if exp.type == 'call' then
            local func = getFunction(exp.name)
            local bad = bad_natives_in_globals[exp.name]
            if bad and func.native then
                if bad == 'crash' then
                    parserWarning(lang.parser.WARNING_CRASH_NATIVE_IN_GLOBAL:format(exp.name))
                end
                if bad == 'null' then
                    parserWarning(lang.parser.WARNING_NULL_NATIVE_IN_GLOBAL:format(exp.name))
                end
            end
        end
    end
    local global = {
        file = file,
        line = linecount,
        constant = constant,
        type = type,
        vtype = type,
        array = array,
        name = name,
        _set = true,
    }
    globals[name] = global
    return global
end

function parser.LocalDef(type, array, name)
    validName(name)
    newNameCheckFunctions(name)
    newNameCheckTypes(name)
    checkType(type)
    if array == '' then
        array = nil
    else
        array = true
    end
    checkLocalWithArgs(name, type, array)
    checkLocalWithGlobals(name, type, array)
    if array then
        if type == 'code' then
            parserError(lang.parser.ERROR_CODE_ARRAY)
        end
    end
    local loc = {
        file = file,
        line = linecount,
        type = type,
        vtype = type,
        array = array,
        name = name,
    }
    state.locals[name] = loc
    local gvar = state.exploit[name] or state.globals[name]
    if gvar then
        state.exploit[name] = loc
        local reDef = {}
        if gvar.type ~= type then
            reDef[#reDef+1] = lang.parser.WARNING_REDEFINED_TO:format(type)
        end
        if gvar.array ~= array then
            if array then
                reDef[#reDef+1] = lang.parser.WARNING_REDEFINED_AS_ARRAY
            else
                reDef[#reDef+1] = lang.parser.WARNING_REDEFINED_AS_NOT_ARRAY
            end
        end
        if #reDef > 0 then
            local reDefText = table.concat(reDef, lang.parser.WARNING_REDEFINE_CONCAT)
            parserWarning(lang.parser.WARNING_REDEFINE_GLOBAL:format(name, reDefText, loc.file, loc.line))
        end
    end
    return loc
end

function parser.Local(loc, exp)
    if exp then
        loc._set = true
        if loc.array then
            parserError(lang.parser.ERROR_ARRAY_INIT)
        end
        if not isExtends(exp.vtype, loc.vtype) then
            parserError(lang.parser.ERROR_SET_TYPE:format(loc.name, loc.vtype, exp.vtype))
        end
        if state.currentFunction and exp.type == 'call' then
            if state.currentFunction.name == exp.name then
                parserError(lang.parser.ERROR_LOCAL_RECURSION)
            end
        end
    end
    return loc
end

function parser.Point()
    return file, linecount
end

function parser.Action(file, line, ast)
    if not ast then
        return
    end
    ast.file = file
    ast.line = line
    return ast
end

function parser.ECall(name, ...)
    local func = getFunction(name)
    local call = {
        type = 'call',
        vtype = func.vtype,
        name = name,
        ...,
    }
    checkCall(func, call)
    return call
end

function parser.Set(name, ...)
    local var, source = getVar(name)
    if select('#', ...) == 1 then
        local exp = ...
        checkSet(var, source, false, exp)
        var._set = true
    else
        local index, exp = ...
        checkSet(var, source, true, exp)
    end
end

function parser.Return()
    local func = state.currentFunction
    if func then
        local t1 = func.vtype
        if t1 then
            parserError(lang.parser.ERROR_MISS_RETURN:format(func.name, t1))
        end
    end
end

function parser.ReturnExp(exp)
    fixRB()
    local func = state.currentFunction
    if func then
        local exploitText = ''
        if exp._var then
            exploitText = getExploitText(exp._var)
        end
        local t1 = func.vtype
        local t2 = exp.vtype
        if t1 then
            if not isExtends(t2, t1) then
                parserRB(lang.parser.ERROR_RETURN_TYPE:format(func.name, t1, t2) .. exploitText)
            end
            if t1 == 'real' and t2 == 'integer' then
                parserRB(lang.parser.ERROR_RETURN_TYPE:format(func.name, t1, t2) .. exploitText)
            end
        else
            parserError(lang.parser.ERROR_WASTE_RETURN:format(func.name, t2))
        end
        if func.constant then
            if exp._var and not exp._var._set then
                parserWarning(lang.parser.ERROR_CONSTANT_UNINIT:format(func.name) .. exploitText, 'runtime')
            end
        end
    end
    returnOneTime()
end

function parser.Exit(exp)
    if state.loop == 0 then
        parserError(lang.parser.ERROR_EXITWHEN)
    end
end

function parser.Logic(chunks, m)
    chunks.endline = linecount
    chunks.type = 'if'
    if not m then
        local start = chunks[1]
        parserError(lang.parser.ERROR_ENDIF:format(start.line))
    end
    local stack = state.returnStack
    if stack then
        state.returnStack = stack - 1
        if state.returnTimes[stack] == 0 then
            -- 所有逻辑分支中都进行了返回，则视为一个返回
            returnOneTime()
        end
    end
    return chunks
end

function parser.IfStart()
    local stack = state.returnStack
    if stack then
        stack = stack + 1
        state.returnStack = stack
        state.returnTimes[stack] = 2 -- if 和 else 各需要一个返回时间
        state.returnMarks[stack] = false
    end
    return file, linecount
end

function parser.If(file, line, condition, ...)
    return {
        line = line,
    }
end

function parser.ElseifStart()
    local stack = state.returnStack
    if stack then
        state.returnTimes[stack] = state.returnTimes[stack] + 1
        state.returnMarks[stack] = false
    end
    return file, linecount
end

function parser.ElseStart()
    local stack = state.returnStack
    if stack then
        state.returnMarks[stack] = false
    end
    return file, linecount
end

function parser.LoopStart()
    state.loop = state.loop + 1
    return linecount
end

function parser.Loop(line, chunks, m)
    if not m then
        parserError(lang.parser.ERROR_ENDLOOP:format(line))
    end
    state.loop = state.loop - 1
end

function parser.NArgs(takes)
    local args = {}
    for i = 1, #takes, 2 do
        local arg = {
            type  = takes[i],
            vtype = takes[i],
            name  = takes[i+1],
        }
        args[#args+1] = arg
    end
    return args
end

function parser.FArgs(takes)
    local args = {}
    for i = 1, #takes, 2 do
        local arg = {
            type  = takes[i],
            vtype = takes[i],
            name  = takes[i+1],
            _set = true,
        }
        args[#args+1] = arg
        state.args[arg.name] = arg
        checkArgWithGlobals(arg.name, arg.type)
    end
    return args
end

function parser.Native(file, line, constant, name, args, returns)
    validName(name)
    newName(name)
    checkType(returns)
    checkArgs(args)
    local func = {
        file = file,
        line = line,
        type = 'function',
        vtype = returns,
        native = true,
        constant = constant ~= '' or nil,
        name = name,
        args = args,
        returns = returns,
    }
    state.functions[name] = func
    ast.functions[#ast.functions+1] = func
    return func
end

function parser.FunctionStart(constant, name, args, returns)
    validName(name)
    newName(name)
    checkType(returns)
    checkArgs(args)
    local func = {
        file = file,
        line = linecount,
        type = 'function',
        vtype = returns,
        constant = constant ~= '' or nil,
        name = name,
        args = args,
        returns = returns,
    }
    state.functions[name] = func
    state.currentFunction = func
    if returns then
        state.returnTimes[1] = 1
        state.returnMarks[1] = false
        state.returnStack = 1
        state.returnAny = false
    end
    ast.functions[#ast.functions+1] = func
end

function parser.FunctionBody(locals, actions)
    local func = state.currentFunction
    func.locals = locals
end

function parser.FunctionEnd(m)
    local func = state.currentFunction
    func.endline = linecount
    state.currentFunction = nil
    if m == '' then
        parserError(lang.parser.ERROR_ENDFUNCTION:format(func.line))
    end
    local locals = state.locals
    local args = state.args
    for k in pairs(locals) do
        locals[k] = nil
    end
    for k in pairs(args) do
        args[k] = nil
    end
    finishRB()
    if func.returns and state.returnTimes[1] > 0 then
        if state.returnAny then
            parserError(lang.parser.ERROR_RETURN_IN_ALL:format(func.name, func.returns))
        else
            parserError(lang.parser.ERROR_MISS_RETURN:format(func.name, func.returns))
        end
    end
    return func
end

function parser.Chunk(chunk)
    return chunk
end

-- 收集错误
function parser.returnAsReturns()
    parserError(lang.parser.ERROR_RETURN_AS_RETURNS)
end

function parser.setAsCall()
    parserError(lang.parser.ERROR_SET_AS_CALL)
end

function parser.callAsSet()
    parserError(lang.parser.ERROR_CALL_AS_SET)
end

function parser.constantLocal()
    parserError(lang.parser.ERROR_CONSTANT_LOCAL)
end

function parser.typeInFunction()
    parserError(lang.parser.ERROR_TYPE_IN_FUNCTION)
end

function parser.localInFunction()
    parserError(lang.parser.ERROR_LOCAL_IN_FUNCTION)
end

return function (jass_, file_, option_)
    ast = {
        functions = {},
    }
    jass = jass_
    file = file_
    linecount = 1

    option = option_ or {}
    state = option.state
    errors = option.errors or {}
    option.errors = errors

    if not state then
        state = {}
        option.state = state
        state.types = {
            null    = {type = 'type'},
            handle  = {type = 'type'},
            code    = {type = 'type'},
            integer = {type = 'type'},
            real    = {type = 'type'},
            boolean = {type = 'type'},
            string  = {type = 'type'},
        }
        state.globals = {}
        state.functions = {}
        state.locals = {}
        state.args = {}
        state.loop = 0
        state.returnTimes = {}
        state.returnMarks = {}
        state.returnStack = nil
    end
    state.exploit = {}
    newCache()
    local gram, err = grammar(jass, file, option.mode, parser)
    errors[#errors+1] = err
    return errors
end
