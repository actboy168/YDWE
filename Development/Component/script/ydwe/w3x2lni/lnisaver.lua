local storm = require 'virtual_storm'
local process = require 'process'
local root = fs.ydwe_path()
local dev = fs.ydwe_devpath()

local function copy_files(source, target)
    if not fs.exists(source) then
        return
    end
    if fs.exists(target) then
        fs.remove_all(target)
    end
    if fs.is_directory(source) then
        fs.create_directories(target)
        for path in source:list_directory() do
            local name = path:filename()
            copy_files(path, target / name)
        end
    else
        fs.copy_file(source, target, true)
    end
end

return function (map_path)
    if map_path:filename():string() ~= '.w3x' then
        return true
    end

    fs.create_directories(root / 'backups')
    fs.copy_file(map_path, root / 'backups' / 'lni_backup.w3x', true)
    fs.copy_file(dev / 'plugin' / 'w3x2lni' / 'script' / 'core' / '.w3x', map_path, true)

    local current_dir = dev / 'plugin' / 'w3x2lni' / 'script'
    local command_line = ('"%s" -e"%s" "%s" %s'):format(
        (root / 'bin' / 'lua.exe'):string(),
        ([[package.cpath = '${YDWE}\\bin\\?.dll;${YDWE}\\bin\\modules\\?.dll';package.path = '${DEV}\\?.lua;${DEV}\\?\\init.lua']]):gsub('${YDWE}', root:string():gsub('\\', '\\\\')):gsub('${DEV}', current_dir:string():gsub('\\', '\\\\')),
        (current_dir / 'gui' / 'mini.lua'):string(),
        ('"lni" "%s" "%s"'):format((root / 'backups' / 'lni_backup.w3x'):string(), map_path:parent_path():string())
    )
    local p = process()
    p:set_console 'disable'
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
    if dev ~= root then
        copy_files(dev / 'plugin' / 'w3x2lni' / 'log', root / 'logs' / 'w3x2lni')
        fs.remove_all(dev / 'plugin' / 'w3x2lni' / 'log')
    end
    if err == '' then
        return true
    else
        log.error(err)
        return false
    end
end
