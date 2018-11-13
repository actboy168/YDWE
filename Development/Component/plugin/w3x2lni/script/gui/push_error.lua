fs = require 'bee.filesystem'
local process = require 'bee.subprocess'
local root = fs.current_path()

return function (err)
    local app = root:parent_path() / 'bin' / 'w3x2lni-lua.exe'
    local script = root / 'crashreport' / 'init.lua'
    local p, stdin = process.spawn {
        app:string(),
        '-E',
        '-e', ('package.cpath=[[%s]]'):format(package.cpath),
        '-e', ('package.path=[[%s]]'):format(package.path),
        script:string(),
        console = 'disable',
        stdin = true,
        cwd = root:string()
    }
    if not p then
        return
    end
    stdin:write(err)
    stdin:close()
    p:wait()
end
