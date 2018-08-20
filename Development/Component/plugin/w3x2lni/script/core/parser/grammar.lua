local lpeg = require 'lpeg'
local lang = require 'lang'

local tonumber = tonumber
local table_concat = table.concat
local messager

lpeg.locale(lpeg)

local S = lpeg.S
local P = lpeg.P
local R = lpeg.R
local C = lpeg.C
local V = lpeg.V
local Cg = lpeg.Cg
local Ct = lpeg.Ct
local Cc = lpeg.Cc
local Cs = lpeg.Cs
local Cp = lpeg.Cp
local Cmt = lpeg.Cmt

local jass
local file
local comments
local line_count
local line_pos

local function errorpos(pos, str)
    local endpos = jass:find('[\r\n]', pos) or (#jass+1)
    local sp = (' '):rep(pos-line_pos)
    local line = ('%s\r\n%s^'):format(jass:sub(line_pos, endpos-1), sp)
    error(lang.parser.ERROR_POS:format(str, file, line_count, line))
end

local function err(str)
    return Cp() / function(pos)
        errorpos(pos, str)
    end
end

local function newline(pos)
    line_count = line_count + 1
    line_pos = pos
end

local function comment(str)
    if comments[line_count] then
        messager(lang.parser.CONFLICT_COMMENT_LINE .. line_count)
        messager(comments[line_count])
        messager(str)
    end
    comments[line_count] = str
end

local w = (1-S' \t\r\n()[]')^0

local function expect(p, ...)
    if select('#', ...) == 1 then
        local str = ...
        return p + w * err(str)
    else
        local m, str = ...
        return p + m * err(str)
    end
end

local function keyvalue(key, value)
    return Cg(Cc(value), key)
end

local function currentline()
    return Cg(P(true) / function() return file end,       'file')
         * Cg(P(true) / function() return line_count end, 'line')
end

local function endline()
    return Cg(P(true) / function() return line_count end, 'endline')
end

local function binary(...)
    local e1, op = ...
    if not op then
        return e1
    end
    local args = {...}
    local e1 = args[1]
    for i = 2, #args, 2 do
        op, e2 = args[i], args[i+1]
        e1 = {
            type = op,
            [1]  = e1,
            [2]  = e2,
        }
    end
    return e1
end

local function unary(...)
    local e1, op = ...
    if not op then
        return e1
    end
    local args = {...}
    local e1 = args[#args]
    for i = #args - 1, 1, -1 do
        op = args[i]
        e1 = {
            type = op,
            [1]  = e1,
        }
    end
    return e1
end

local nl  = (P'\r\n' + S'\r\n') * Cp() / newline
local com = P'//' * C((1-nl)^0) / comment
local sp  = (S' \t' + P'\xEF\xBB\xBF' + com)^0
local sps = (S' \t' + P'\xEF\xBB\xBF' + com)^1
local cl  = com^0 * nl
local spl = sp * cl
local finish = P'\0' * P(1)^0

local Keys = {'globals', 'endglobals', 'constant', 'native', 'array', 'and', 'or', 'not', 'type', 'extends', 'function', 'endfunction', 'nothing', 'takes', 'returns', 'call', 'set', 'return', 'if', 'endif', 'elseif', 'else', 'loop', 'endloop', 'exitwhen'}
for _, key in ipairs(Keys) do
    Keys[key] = true
end

local Id = P{
    'Def',
    Def  = C(V'Id') * Cp() / function(id, pos) if Keys[id] then errorpos(pos-#id, lang.parser.ERROR_KEY_WORD:format(id)) end end,
    Id   = R('az', 'AZ') * R('az', 'AZ', '09', '__')^0,
}

local Cut = #(1-R('az', 'AZ', '09', '__')) + (-P(1))
local function Whole(word)
    return P(word) * Cut
end

local Null = Ct(keyvalue('type', 'null') * P'null')

local Bool = P{
    'Def',
    Def   = Ct(keyvalue('type', 'boolean') * Cg(V'True' + V'False', 'value')),
    True  = Whole'true' * Cc(true),
    False = Whole'false' * Cc(false),
}

local Str = P{
    'Def',
    Def  = Ct(keyvalue('type', 'string') * Cg(V'Str', 'value')),
    Str  = '"' * Cs((nl + V'Char')^0) * '"',
    Char = V'Esc' + '\\' * err(lang.parser.ERROR_ESC) + (1-P'"'),
    Esc  = P'\\b'
         + P'\\t'
         + P'\\r'
         + P'\\n'
         + P'\\f'
         + P'\\"'
         + P'\\\\',
}

local Real = P{
    'Def',
    Def  = Ct(keyvalue('type', 'real') * Cg(V'Real', 'value')),
    Real = P'-'^-1 * sp * V'Char',
    Char  = (P'.' * expect(R'09'^1, lang.parser.ERROR_REAL) + R'09'^1 * P'.' * R'09'^0),
}

local Int = P{
    'Def',
    Def    = Ct(keyvalue('type', 'integer') * Cg(V'Int', 'value')),
    Int    = V'Neg' * (V'Int16' + V'Int8' + V'Int10' + V'Int256') / function(neg, n) return neg and -n or n end,
    Neg    = Cc(true) * P'-' * sp + Cc(false),
    Int8   = (P'0' * R'07'^1) / function (n) return tonumber(n, 8) end,
    Int10  = (P'0' + R'19' * R'09'^0) / tonumber,
    Int16  = (P'$' + P'0' * S'xX') * expect(R('af', 'AF', '09')^1 / function(n) return tonumber('0x'..n) end, lang.parser.ERROR_INT16),
    Int256 = "'" * expect((V'C4' + V'C1') * "'", lang.parser.ERROR_INT256_COUNT),
    C4     = V'C4W' * V'C4W' * V'C4W' * V'C4W' / function(n) return ('>I4'):unpack(n) end,
    C4W    = expect(1-P"'"-P'\\', '\\' * P(1), lang.parser.ERROR_INT256_ESC),
    C1     = ('\\' * expect(V'Esc', P(1), lang.parser.ERROR_ESC) + C(1-P"'")) / function(n) return ('I1'):unpack(n) end,
    Esc    = P'b' / function() return '\b' end 
           + P't' / function() return '\t' end
           + P'r' / function() return '\r' end
           + P'n' / function() return '\n' end
           + P'f' / function() return '\f' end
           + P'"' / function() return '\"' end
           + P'\\' / function() return '\\' end,
}

local Value = sp * (Null + Bool + Str + Real + Int) * sp

local Exp = P{
    'Def',
    
    -- 由低优先级向高优先级递归
    Def      = V'And',

    And      = V'Or'      * (C(Whole'and')             * sp    * V'Or')^0      / binary,
    Or       = V'Compare' * (C(Whole'or')              * sp    * V'Compare')^0 / binary,
    Compare  = V'Not'     * (C(S'><=!' * P'=' + S'><') * sp    * V'Not')^0     / binary,
    Not      = sp         * (C(Whole'not')             * sp)^0 * V'AddSub'     / unary,
    AddSub   = V'MulDiv'  * (C(S'+-')                  * sp    * V'MulDiv')^0  / binary,
    MulDiv   = V'Exp'     * (C(S'*/')                  * sp    * V'Exp')^0     / binary,

    Exp   = V'Paren' + V'Code' + V'Call' + Value + V'Neg' + V'Vari' + V'Var',
    Paren = sp * '(' * V'Def' * ')' * sp,

    Code  = Ct(keyvalue('type', 'code')  * sp * Whole'function' * sps * Cg(Id, 'name') * sp),
    Call  = Ct(keyvalue('type', 'call')  * sp * Cg(Id, 'name') * sp * '(' * V'Args' * ')' * sp),
    Vari  = Ct(keyvalue('type', 'vari')  * sp * Cg(Id, 'name') * sp * '[' * Cg(V'Def', 1) * ']' * sp),
    Var   = Ct(keyvalue('type', 'var')   * sp * Cg(Id, 'name') * sp),
    Neg   = Ct(keyvalue('type', 'neg')   * sp * '-' * sp * Cg(V'Exp', 1)),

    Args  = V'Def' * (',' * V'Def')^0 + sp,
}

local Type = P{
    'Def',
    Def  = Ct(sp * Whole'type' * keyvalue('type', 'type') * currentline() * expect(sps * Cg(Id, 'name'), lang.parser.ERROR_VAR_TYPE) * expect(V'Ext', lang.parser.ERROR_EXTENDS_TYPE)),
    Ext  = sps * Whole'extends' * sps * Cg(Id, 'extends'),
}

local Global = P{
    'Global',
    Global = Ct(sp * Whole'globals' * keyvalue('type', 'globals') * currentline() * V'Vals' * V'End'),
    Vals   = (spl + V'Def' * spl)^0,
    Def    = Ct(currentline() * sp
        * (Whole'constant' * sps * keyvalue('constant', true) + P(true))
        * Cg(Id, 'type') * sps
        * (Whole'array' * sps * keyvalue('array', true) + P(true))
        * Cg(Id, 'name')
        * (sp * '=' * Cg(Exp) + P(true))
        ),
    End    = expect(sp * Whole'endglobals', lang.parser.ERROR_ENDGLOBALS),
}

local Local = P{
    'Def',
    Def = Ct(currentline() * sp
        * Whole'local' * sps
        * Cg(Id, 'type') * sps
        * (Whole'array' * sps * keyvalue('array', true) + P(true))
        * Cg(Id, 'name')
        * (sp * '=' * Cg(Exp) + P(true))
        ),
}

local Line = P{
    'Def',
    Def    = sp * (V'Call' + V'Set' + V'Seti' + V'Return' + V'Exit'),
    Call   = Ct(keyvalue('type', 'call') * currentline() * Whole'call' * sps * Cg(Id, 'name') * sp * '(' * V'Args' * ')' * sp),
    Args   = Exp * (',' * Exp)^0 + sp,
    Set    = Ct(keyvalue('type', 'set')  * currentline() * Whole'set'  * sps * Cg(Id, 'name') * sp * '=' * Exp),
    Seti   = Ct(keyvalue('type', 'seti') * currentline() * Whole'set'  * sps * Cg(Id, 'name') * sp * '[' * Cg(Exp, 1) * ']' * sp * '=' * Cg(Exp, 2)),

    Return = Ct(keyvalue('type', 'return') * currentline() * Whole'return'   * (Cg(Exp, 1) + P(true))),
    Exit   = Ct(keyvalue('type', 'exit')   * currentline() * Whole'exitwhen' * Cg(Exp, 1)),
}

local Logic = P{
    'Def',
    Def      = V'If' + V'Loop',

    If       = Ct(keyvalue('type', 'if') * currentline() * sp
            * V'Ifif'
            * V'Ifelseif'^0 
            * V'Ifelse'^-1
            * sp * 'endif' * endline()
            ),
    Ifif     = Ct(keyvalue('type', 'if')     * currentline() * sp * Whole'if'     * Cg(Exp, 'condition') * Whole'then' * spl * V'Ifdo'),
    Ifelseif = Ct(keyvalue('type', 'elseif') * currentline() * sp * Whole'elseif' * Cg(Exp, 'condition') * Whole'then' * spl * V'Ifdo'),
    Ifelse   = Ct(keyvalue('type', 'else')   * currentline() * sp * Whole'else'   * spl * V'Ifdo'),
    Ifdo     = (spl + V'Def' + Line * spl)^0,

    Loop     = Ct(keyvalue('type', 'loop') * currentline() * sp
            * Whole'loop' * spl
            * (spl + V'Def' + Line * spl)^0
            * sp * Whole'endloop' * endline()
            ),
}

local Function = P{
    'Def',
    Def      = Ct(keyvalue('type', 'function') * currentline() * (V'Common' + V'Native')),
    Native   = sp * (Whole'constant' * keyvalue('constant', true) + P(true)) * sp * Whole'native' * keyvalue('native', true) * V'Head',
    Common   = sp * (Whole'constant' * keyvalue('constant', true) + P(true)) * sp * Whole'function' * V'Head' * spl * V'Content' * V'End',
    Head     = sps * Cg(Id, 'name') * sps * Whole'takes' * sps * V'Takes' * sps * Whole'returns' * sps * V'Returns',
    Takes    = (Whole'nothing' + Cg(V'Args', 'args')),
    Args     = Ct(sp * V'Arg' * (sp * ',' * sp * V'Arg')^0),
    Arg      = Ct(Cg(Id, 'type') * sps * Cg(Id, 'name')),
    Returns  = Whole'nothing' + Cg(Id, 'returns'),
    Content  = sp * Cg(V'Locals', 'locals') * V'Lines',
    Locals   = Ct((spl + Local * spl)^0),
    Lines    = (spl + Logic * spl + Line * spl)^0,
    End      = expect(sp * Whole'endfunction', lang.parser.ERROR_ENDFUNCTION) * endline(),
}

local pjass = expect(sps + cl + finish + Type + Function + Global, P(1), lang.parser.SYNTAX_ERROR)^0

local mt = {}
setmetatable(mt, mt)

mt.Value  = Value
mt.Id     = Id
mt.Exp    = Exp
mt.Global = Global
mt.Local  = Local
mt.Line   = Line
mt.Logic  = Logic
mt.Function = Function

function mt:__call(_jass, _file, _messager, mode)
    jass = _jass
    file = _file
    messager = _messager
    comments = {}
    line_count = 1
    line_pos = 1
    lpeg.setmaxstack(1000)
    
    if mode then
        return Ct(expect(mt[mode] + spl, P(1), lang.parser.SYNTAX_ERROR)^0):match(_jass)
    else
        return Ct(pjass):match(_jass), comments
    end
end

return mt
