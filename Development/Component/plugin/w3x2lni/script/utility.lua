if io.load then
    return
end

local uni = require 'ffi.unicode'
local real_io_open = io.open

function io.open(path, ...)
    return real_io_open(uni.u2a(path:string()), ...)
end

function io.load(file_path)
    local f, e = io.open(file_path, "rb")
    if f then
        if f:read(3) ~= '\xEF\xBB\xBF' then
            f:seek('set')
        end
        local content = f:read 'a'
        f:close()
        return content
    else
        return false, e
    end
end

function io.save(file_path, content)
    local f, e = io.open(file_path, "wb")

    if f then
        f:write(content)
        f:close()
        return true
    else
        return false, e
    end
end
