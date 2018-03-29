
local function get_version(str)
    local r = {}
    str:gsub('[^%.]+', function (w) r[#r+1] = tonumber(w) end)
    local ver = 0
    for _, v in ipairs(r) do
        ver  = ver * 100 + v
    end
    return ver
end

local function get_debugger()
    local curdbg
    local curver
    local path = fs.path(os.getenv('USERPROFILE')) / '.vscode' / 'extensions'
    for extpath in path:list_directory() do
        local dir = extpath:filename():string()
        if dir:sub(1, 20) == 'actboy168.lua-debug-' then
            local dbg = extpath / 'debugger.dll'
            if fs.exists(dbg) then
                local ver = get_version(dir:sub(21), '.')
                if not curver or curver < ver then
                    curdbg = dbg
                    curver = ver
                end
            end
        end
    end
    return curdbg
end

local debugger
local dbgpath = get_debugger()
if dbgpath then
    local init = package.loadlib(dbgpath:string(), 'luaopen_debugger')
    if init then
        debugger = init()
        debugger.listen('127.0.0.1', 4279, true)
        log.info('enable debugger path:', dbgpath)
        log.info('enable debugger port:', debugger:port())
        return debugger
    end
end
