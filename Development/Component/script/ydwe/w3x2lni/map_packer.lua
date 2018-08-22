local process = require 'process'
local root = fs.ydwe_path()
local dev = fs.ydwe_devpath()
local i18n = require "i18n"

local function trans_command(cmd)
    local str = cmd:gsub([[(\\?)$]], function (str)
        if str == [[\]] then
            return [[\\]]
        end
    end)
    return '"' .. str .. '"'
end


local function do_command(script, ...)
    local cmds = {...}
    for i, str in ipairs(cmds) do
        cmds[i] = trans_command(str)
    end
    local current_dir = dev / 'plugin' / 'w3x2lni' / 'script'
    local command_line = ([=[
"%s" -E -e"package.cpath=[[%s]];package.path=[[%s;%s]]" "%s" %s]=]
    ):format(
        (root / 'bin' / 'lua.exe'):string(),
        (root / 'bin' / 'modules' / '?.dll'):string(),
        (current_dir / '?.lua'):string(),
        (current_dir / '?' / 'init.lua'):string(),
        (current_dir / script):string(),
        table.concat(cmds, ' ')
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
    if err == '' then
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
