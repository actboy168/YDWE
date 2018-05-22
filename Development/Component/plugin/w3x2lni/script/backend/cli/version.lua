local cl = require 'share.changelog'
local messager = require 'share.messager'

return function()
    messager.raw('w3x2lni version ' .. cl[1].version)
    local ok, gl = pcall(require, 'share.gitlog')
    if ok then
        messager.raw('\ncommit: ' .. gl.commit)
        messager.raw('\ndate: ' .. gl.date)
    end
end
