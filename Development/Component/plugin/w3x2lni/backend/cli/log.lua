require 'filesystem'
require 'utility'
local messager = require 'share.messager'

return function ()
    local root = fs.current_path()
    local s = {}
    local logfile = (root:parent_path() / 'log' / 'report.log'):string()
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
