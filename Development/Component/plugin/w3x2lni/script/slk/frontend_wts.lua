local lpeg = require 'lpeg'

local function search_string(buf)
    local line_count = 1
    lpeg.locale(lpeg)
    local S = lpeg.S
    local P = lpeg.P
    local R = lpeg.R
    local C = lpeg.C
    local V = lpeg.V
    local Ct = lpeg.Ct
    local Cg = lpeg.Cg
    local Cp = lpeg.Cp

    local function newline()
        line_count = line_count + 1
    end

    local function getline()
        return line_count
    end

    local bom    = P'\xEF\xBB\xBF'
    local nl     = (P'\r\n' + S'\r\n') / newline
    local char   = nl + P(1)
    local com    = P'//' * (1-nl)^0 * nl^-1
    local int    = P'0' + R'19' * R'09'^0
    local define = P
    {
        'define',
        define = Ct(V'head' * V'comline'^-1 * V'body'),
        head   = P'STRING ' * Cg(int / tonumber, 'index') * Cg(Cp() / getline, 'line') * nl,
        comline= com * (char - V'start')^0,
        body   = V'start' * Cg(V'text', 'text') * V'finish',
        start  = P'{' * nl,
        finish = nl * P'}' * nl^0,
        text   = (char - V'finish' * (V'sdefine' + -P(1)))^0,
        sdefine= V'head' * V'comline'^-1 * V'sbody',
        sbody  = V'start' * V'stext' * V'finish',
        stext  = (char - V'finish')^0,
    }

    local function err(str)
        return ((1-nl)^1 + P(1)) / function(c)
            error(('line[%d]: %s:\n===========================\n%s\n==========================='):format(line_count, str, c))
        end
    end

    local searcher = Ct(bom^-1 * (nl + com)^0 * (define + err'syntax error')^0)
    local result = searcher:match(buf)
    return result
end

return function (w2l, buf)
    local tbl = { mark = {} }
    if not buf then
        return tbl
    end
    local result = search_string(buf)
    for _, t in ipairs(result) do
        local index, text = t.index, t.text
        if text:find('}', 1, false) then
            message('-report|2警告', '文本不能包含字符"}"')
            message('-tip', (text:sub(1, 1000):gsub('\r\n', ' ')))
        end
        tbl[index] = t
    end
    return tbl
end
