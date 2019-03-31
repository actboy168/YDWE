
require 'filesystem'
local check_lni_mark = require 'share.check_lni_mark'
local base = require 'backend.base_path'

return function ()
    local path
    local cur = base
    while true do
        if fs.exists(cur / '.w3x') then
            if check_lni_mark(io.load(cur / '.w3x')) then
                path = cur
                break
            else
                return nil, 'lni mark failed'
            end
        end
        if cur == cur:parent_path() then
            break
        end
        cur = cur:parent_path()
    end
    if not path then
        return nil, 'no lni'
    end

    if path:filename():string() == '.w3x' then
        if check_lni_mark(io.load(path)) then
            return fs.absolute(path:parent_path())
        else
            return fs.absolute(path), 'lni mark failed'
        end
    end
    return fs.absolute(path)
end
