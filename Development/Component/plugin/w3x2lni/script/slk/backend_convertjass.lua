local lpeg = require 'lpeg'
local w2l
local wts

local function fwts(str)
    return w2l:load_wts(wts, str, 1023, '脚本里的文本长度超过1023字符', function(str)
        return str:gsub('\\', '\\\\'):gsub('"', '\\"')
    end)
end

lpeg.locale(lpeg)
local S = lpeg.S
local P = lpeg.P
local R = lpeg.R
local C = lpeg.C
local Cs = lpeg.Cs

local nl1  = P'\r\n' + S'\r\n'
local com  = P'//' * (1-nl1)^0
local nl   = com^0 * nl1
local quo  = P'"'
local esc  = P'\\'
local num  = R'09'
local str1 = esc * P(1) + (1-quo)
local str  = quo * (nl1 + str1)^0 * quo
local wst  = quo * ('TRIGSTR_' * num^3 / fwts) * quo
local mtch = C(nl) + Cs(wst) + C(str)
local any  = C((1 - mtch)^1)

local pjass = (mtch + any)^0 / function(...)
    return table.concat {...}
end
return function (w2l_, archive, wts_)
    local name = 'war3map.j'
    local buf = archive:get(name)
    if not buf then
        name = 'scripts\\war3map.j'
        buf = archive:get(name)
        if not buf then
            return
        end
    end
    w2l = w2l_
    wts = wts_
    archive:set(name, pjass:match(buf))
end
