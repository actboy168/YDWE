local command = require 'share.command'
local lni = require 'lni'
local input_path = require 'share.input_path'
local global_config = require 'share.config'
require 'utility'
require 'filesystem'

local root = fs.current_path()

local function output_path(path)
    if not path then
        return nil
    end
    path = fs.path(path)
    if not path:is_absolute() then
        if _W2L_DIR then
            path = fs.path(_W2L_DIR) / path
        else
            path = root:parent_path() / path
        end
    end
    return fs.absolute(path)
end

return function (mode)
    local config = { mode = mode }
    local input = input_path(command[2])
    local output = output_path(command[3])

    global_config:open_map(input)
    for k, v in pairs(global_config.global) do
        config[k] = v
    end
    if global_config[config.mode] then
        for k, v in pairs(global_config[config.mode]) do
            config[k] = v
        end
    end
    config.input = input
    config.output = output
    
    return config
end
