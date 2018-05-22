local function send(type, args)
    local content = ('{type=%q,args=%s}\r\n'):format(type, args)
    io.stdout:write(('Content-Length: %d\r\n%s'):format(#content, content))
end

local function execute(s)
    return assert(load('return (' .. s .. ')', '=(protocol)', 't', {}))()
end

local function recv(s, bytes)
    bytes = bytes or ''
    s.bytes = s.bytes and (s.bytes .. bytes) or bytes
    while true do
        if s.length then
            if s.length <= #s.bytes then
                local res = s.bytes:sub(1, s.length)
                s.bytes = s.bytes:sub(s.length + 1)
                s.length = nil
                return execute(res)
            end
            return
        end
        local pos = s.bytes:find('\r\n', 1, true)
        if not pos then
            return
        end
        if pos <= 15 or s.bytes:sub(1, 16) ~= 'Content-Length: ' then
            return error('Invalid protocol.')
        end
        local length = tonumber(s.bytes:sub(17, pos))
        if not length then
            return error('Invalid protocol.')
        end
        s.bytes = s.bytes:sub(pos + 2)
        s.length = length
    end
end

return {
    send = send,
    recv = recv,
}
