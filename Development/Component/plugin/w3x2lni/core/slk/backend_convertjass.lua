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

local function convert_wts(name)
    local buf = w2l:file_load('map', name)
    if buf then
        w2l:file_save('map', name, pjass:match(buf))
    end
end

local function convert_mark(name)
    local buf = w2l:file_load('map', name)
    if buf then
        local time = ('//W3x2lni Data: %s.%03.f'):format(os.date '%Y-%m-%d %H:%M:%S', (os.clock() % 1) * 1000)
        if buf:sub(1, 15) == '//W3x2lni Data:' then
            local pos = buf:find('[\r\n]')
            if pos then
                local new_buf = time .. buf:sub(pos)
                w2l:file_save('map', name, new_buf)
                return
            end
        end
        local new_buf = time .. '\r\n' .. buf
        w2l:file_save('map', name, new_buf)
    end
end

return function (w2l_, wts_)
    w2l = w2l_
    wts = wts_
    convert_wts('war3map.j')
    convert_wts('scripts\\war3map.j')
    convert_mark('war3map.j')
    convert_mark('scripts\\war3map.j')
end