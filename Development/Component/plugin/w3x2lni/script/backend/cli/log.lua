require 'filesystem'
require 'utility'
local messager = require 'share.messager'
local root = require 'backend.w2l_path'

return function ()
    local s = {}
    local logfile = (root / 'log' / 'report.log'):string()
    local f = io.open(logfile, 'r')
    if not f then
        return
    end
    for l in f:lines() do
        s[#s+1] = l
        if #s > 50 then
            messager.raw(table.concat(s, '\r\n') .. '\r\n')
            messager.wait()
            s = {}
        end
    end
    f:close()
    messager.raw(table.concat(s, '\r\n') .. '\r\n')
end
