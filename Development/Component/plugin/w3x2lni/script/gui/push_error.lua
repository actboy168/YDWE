require 'filesystem'
local process = require 'process'
local root = fs.current_path()

return function (err)
    local p = process()
    local stdin = p:std_input()
    local app = root:parent_path() / 'bin' / 'w3x2lni-lua.exe'
    local script = root / 'crashreport' / 'init.lua'
    local command = ('"%s" -e "package.cpath=[[%s]];package.path=[[%s]]" "%s"'):format(app:string(), package.cpath, package.path, script:string())
    p:set_console('disable')
    if not p:create(app, command, root) then
        return
    end
    stdin:write(err)
    stdin:close()
    p:wait()
end
