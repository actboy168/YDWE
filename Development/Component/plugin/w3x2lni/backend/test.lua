local unpack_config = require 'backend.unpack_config'
local messager = require 'tool.messager'
local uni = require 'ffi.unicode'
local root = fs.current_path()

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
    local config = unpack_config()
    local input = config.input
    local output = root:parent_path() / 'temp'
    fs.create_directories(output)
    messager.text(('地图路径：%s'):format(input))
    task('lni', input)
end
