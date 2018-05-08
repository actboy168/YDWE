require 'filesystem'
local minizip = require 'minizip'

local function for_directory(path, func, leaf)
    for file in (leaf and (path / leaf) or path):list_directory() do
        local leaf = (leaf and (leaf / file:filename()) or file:filename())
        if fs.is_directory(file) then
            for_directory(path, func, leaf)
        else
            func(leaf)
        end
    end
end

local function loadfile(filename)
    local f, e = io.open(filename:string(), 'rb')
    if not f then error(e) end
    local content = f:read 'a'
    f:close()
    return content
end

return function(dir, zip, filter)
    local z = minizip(zip:string(), 'w')
    for_directory(dir, function(file)
        if not filter or filter(file) then
            z:archive(file:string(), loadfile(dir / file))
        end
    end)
    z:close()
end
