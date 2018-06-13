local unpack_setting = require 'backend.unpack_setting'
local messager = require 'share.messager'
local uni = require 'ffi.unicode'
local root = require 'backend.w2l_path'

local function get_command(...)
    local exe = root / 'w2l'
    local commands = {exe, ...}
    for i, c in ipairs(commands) do
        commands[i] = ('%s'):format(c)
    end
    return table.concat(commands, ' ')
end

local function task(...)
    local command = get_command(...)
    messager.text(('正在执行：%s'):format(command))
    io.popen(command):close()
end

return function ()
    local setting = unpack_setting()
    local input = setting.input
    local output = root / 'temp'
    fs.create_directories(output)
    messager.text(('地图路径：%s'):format(input))
    task('lni', input)
end
