local storm = require 'virtual_storm'
local process = require 'process'
local root = fs.ydwe_path()

return function (map_path)
    if not storm.get_dummy_map() then
        return
    end
    if map_path:filename():string() ~= '.w3x' then
        return true
    end

    local current_dir = root / 'plugin' / 'w3x2lni' / 'script'
    local command_line = ('"%s" -e"%s" "%s" %s'):format(
        (root / 'bin' / 'lua.exe'):string(),
        ([[package.cpath = '${YDWE}\\bin\\?.dll;${YDWE}\\bin\\modules\\?.dll';package.path = '?.lua;?\\init.lua']]):gsub('${YDWE}', root:string():gsub('\\', '\\\\')),
        (current_dir / 'gui' / 'mini.lua'):string(),
        ('"-slk" "%s" "%s"'):format(map_path:string(), map_path:parent_path():string())
    )
    local p = process()
    p:hide_window()
    local stdout, stderr = p:std_output(), p:std_error()
    if not p:create(root / 'bin' / 'lua.exe', command_line, current_dir) then
        log.error(string.format("Executed %s failed", command_line))
        return false
    end
    log.trace(string.format("Executed %s.", command_line))
    local out = stdout:read 'a'
    local err = stderr:read 'a'
    local exit_code = p:wait()
    p:close()
    if err == '' then
        log.info(out)
        return true
    else
        log.error(err)
        return false
    end
end
