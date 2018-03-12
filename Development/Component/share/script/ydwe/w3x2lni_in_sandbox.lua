local sandbox = require 'sandbox'

local function getparent(path)
    if path then
        local pos = path:find [[[/\][^\/]*$]]
        if pos then
            return path:sub(1, pos)
        end
    end
end

local function search_init(name)
    local searchers = package.searchers
    assert(type(searchers) == "table", "'package.searchers' must be a table")
    local msg = ''
    for _, searcher in ipairs(searchers) do
        local f, extra = searcher(name)
        if type(f) == 'function' then
            local root = getparent(extra)
            if not root then
                return error(("module '%s' not found"):format(name))
            end
            return root
        elseif type(f) == 'string' then
            msg = msg .. f
        end
    end
    error(("module '%s' not found:%s"):format(name, msg))
end

local root = search_init('w3x2lni')

return sandbox(root, io.__open, { 
    ['w3xparser'] = require 'w3xparser',
    ['lni']     = require 'lni',
    ['lpeg']      = require 'lpeg',
    ['lml']       = require 'lml',
})
