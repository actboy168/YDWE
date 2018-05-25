local cl = require 'share.changelog'
local messager = require 'share.messager'

return function()
    messager.raw('W3x2Lni v' .. cl[1].version)
    local ok, gl = pcall(require, 'share.gitlog')
    if ok then
        messager.raw('\ncommit: ' .. gl.commit)
        messager.raw('\ndate: ' .. gl.date)
    end
end
