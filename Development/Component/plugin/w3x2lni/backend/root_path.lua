require 'filesystem'

local root = fs.current_path()

return function (path)
    local path = fs.path(path)
    if not path:is_absolute() then
        path = root:parent_path() / path
    end
    return path
end
