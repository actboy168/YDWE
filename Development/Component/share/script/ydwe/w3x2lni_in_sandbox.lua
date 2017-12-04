local loader = {}

local sandbox = require 'sandbox'
local w2l = sandbox('w3x2lni', {
    ['w3xparser'] = require 'w3xparser',
    ['lni-c']     = require 'lni-c',
    ['lpeg']      = require 'lpeg',
    ['loader']    = loader,
    ['io']        = { open = function(filename) return io.open(fs.path(filename)) end },
})

return function (new_loader)
    for k in pairs(loader) do
        loader[k] = nil
    end
    if new_loader then
        for k, v in pairs(new_loader) do
            loader[k] = v
        end
    end
    return w2l
end
