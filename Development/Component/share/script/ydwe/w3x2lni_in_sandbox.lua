local sandbox = require 'sandbox'
local w3x2lni = sandbox('w3x2lni', {
    ['w3xparser'] = require 'w3xparser',
    ['lni-c']     = require 'lni-c',
    ['lpeg']      = require 'lpeg',
    ['io']        = { open = function(filename) return io.open(fs.path(filename)) end },
})

return function ()
    local w2l = w3x2lni()
    w2l:set_messager(function() end)
    return w2l
end
