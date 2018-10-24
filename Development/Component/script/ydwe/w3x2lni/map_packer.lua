local subprocess = require 'subprocess'
local root = fs.ydwe_path()
local dev = fs.ydwe_devpath()
local i18n = require 'i18n'

local function do_command(script, ...)
    local current_dir = dev / 'plugin' / 'w3x2lni' / 'script'
    local args = {
        (root / 'bin' / 'lua.exe'):string(),
        '-E', 
        '-e', ('package.cpath=[[%s]]'):format(
            (root / 'bin' / 'modules' / '?.dll'):string()
        ),
        '-e', ('package.path=[[%s;%s]]'):format(
            (current_dir / '?.lua'):string(),
            (current_dir / '?' / 'init.lua'):string()
        ),
        (current_dir / script):string(),
        {...},
        console = 'disable',
        stdout = true,
        stderr = true,
        cwd = current_dir:string(),
    }
    local process, stdout, stderr = subprocess.spawn(args)
    log.trace(string.format('Executed %s.', sys.tbl_concat(args, ' ')))
    local out = stdout:read 'a'
    local err = stderr:read 'a'
    local exit_code = process:wait()
    log.debug('exit_code', exit_code)
    if exit_code == 0 then
        return true
    else
        log.error(err)
        return false
    end
end

local command = {
    backend = function (...)
        return do_command('backend/init.lua', ...)
    end,
    frontend = function (...)
        return do_command('gui/mini.lua', ...)
    end,
}

local function make_zhCN()
    command.backend('config', 'global.data=zhCN-1.24.4')
    command.backend('config', 'global.data_ui=${YDWE}')
    command.backend('config', 'global.data_meta=${DEFAULT}')
    command.backend('config', 'global.data_wes=${DEFAULT}')
    command.backend('config', 'slk.slk_doodad=true')
end

local function make_enUS()
    command.backend('config', 'global.data=enUS-1.27.1')
    command.backend('config', 'global.data_ui=${DATA}')
    command.backend('config', 'global.data_meta=${DATA}')
    command.backend('config', 'global.data_wes=${DATA}')
    command.backend('config', 'slk.slk_doodad=false')
end

return function (mode, map_path, target_path)
    local lang = i18n.get_language()
    if lang:sub(1, 2) == 'en' then
        make_enUS()
    else
        make_zhCN()
    end
    local result = command.frontend(mode, map_path:string(), target_path:string())
    return result
end
