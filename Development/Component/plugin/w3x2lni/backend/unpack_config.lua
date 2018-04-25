local command = require 'tool.command'
local lni = require 'lni'
local root_path = require 'backend.root_path'
local input_path = require 'tool.input_path'
local create_config = require 'tool.config'
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
    return fs.canonical(path)
end

return function (mode)
    local config = { mode = mode }
    local input = input_path(command[2])
    local output = output_path(command[3])

    local tbl = create_config(input)
    for k, v in pairs(tbl.global) do
        config[k] = v
    end
    if tbl[config.mode] then
        for k, v in pairs(tbl[config.mode]) do
            config[k] = v
        end
    end
    config.input = input
    config.output = output
    
    return config
end
