local loader = require 'loader'
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
local wst  = 'TRIGSTR_' * num^3 / fwts * P'\0'
local mtch = Cs(wst)
local any  = C((1 - mtch)^1)

local pwtg = (mtch + any)^0 / function(...)
    return table.concat {...}
end

return function (w2l_, wts_)
    local name = 'war3map.wtg'
    local buf = loader:map_load(name)
    if not buf then
        return
    end
    w2l = w2l_
    wts = wts_
    loader:map_save(name, pwtg:match(buf))
end
