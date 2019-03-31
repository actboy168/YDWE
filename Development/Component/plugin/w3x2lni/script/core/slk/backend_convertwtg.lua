local lang = require 'lang'
local w2l
local wts

-- TODO 使用更严谨的匹配规则
local lpeg = require 'lpeg'

local function fwts(str)
    return w2l:load_wts(wts, str, 299, lang.script.TEXT_TOO_LONG_IN_WTG, function(str)
        return str:gsub('\\', '\\\\'):gsub('"', '\\"')
    end)
end

lpeg.locale(lpeg)
local S = lpeg.S
local P = lpeg.P
local R = lpeg.R
local C = lpeg.C
local Cs = lpeg.Cs

local num  = R'09'
local wst  = 'TRIGSTR_' * num^3 / fwts * P'\0'
local mtch = Cs(wst)
local any  = C((1 - mtch)^1)

local pwtg = (mtch + any)^0 / function(...)
    return table.concat {...}
end

return function (w2l_, wts_)
    w2l = w2l_
    wts = wts_
    local name = 'war3map.wtg'
    local buf = w2l:file_load('map', name)
    if not buf then
        return
    end
    w2l:file_save('map', name, pwtg:match(buf))
end
