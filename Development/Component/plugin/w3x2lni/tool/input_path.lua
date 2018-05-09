
require 'filesystem'
local check_lni_mark = require 'tool.check_lni_mark'
local root = fs.current_path()

return function (path)
    if path then
        path = fs.path(path)
        if not path:is_absolute() then
            if _W2L_DIR then
                path = fs.path(_W2L_DIR) / path
            else
                path = root:parent_path() / path
            end
        end
    elseif _W2L_MODE == 'CLI' then
        local cur = fs.path(_W2L_DIR)
        while true do
            if check_lni_mark(cur / '.w3x') then
                path = cur
                break
            end
            if cur == cur:parent_path() then
                break
            end
            cur = cur:parent_path()
        end
        if not path then
            return nil
        end
    else
        return nil
    end
    if check_lni_mark(path) then
        path = path:parent_path()
    end
    return fs.canonical(path)
end
