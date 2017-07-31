local w2l
local wts

-- TODO 使用更严谨的匹配规则
local lpeg = require 'lpeg'

local function fwts(str)
    return w2l:load_wts(wts, str, 299, '触发器里的文本长度超过299字符', function(str)
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
local wst  = Cs('TRIGSTR_' * num^3 / fwts * P'\0')

local pwtg = Cs((wst + 1)^0)

return function (w2l_, archive, wts_)
    local name = 'war3map.wtg'
    local buf = archive:get(name)
    if not buf then
        return
    end
    w2l = w2l_
    wts = wts_
    archive:set(name, pwtg:match(buf))
end
