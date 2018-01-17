local sandbox = require 'sandbox'

local function loadlua(name)
    local searchers = package.searchers
    assert(type(searchers) == "table", "'package.searchers' must be a table")
    local msg = ''
    for _, searcher in ipairs(searchers) do
        local f, extra = searcher(name)
        if type(f) == 'function' then
            return f, extra
        elseif type(f) == 'string' then
            msg = msg .. f
        end
    end
    error(("module '%s' not found:%s"):format(name, msg))
end

local w3x2lni = sandbox('w3x2lni', loadlua, {
    ['w3xparser'] = require 'w3xparser',
    ['lni-c']     = require 'lni-c',
    ['lpeg']      = require 'lpeg',
    ['io']        = { open = function(filename) return io.open(fs.path(filename)) end },
})

return w3x2lni
