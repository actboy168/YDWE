local command = require 'backend.command'
local lni = require 'lni'
local get_lni_map = require 'backend.get_lni_map'
local config = require 'share.config'
local lang = require 'share.lang'
local check_lni_mark = require 'share.check_lni_mark'
local base = require 'backend.base_path'
local root = require 'backend.w2l_path'

require 'utility'
require 'filesystem'

local function output_path(path)
    if not path then
        return
    end
    path = fs.path(path)
    if not path:is_absolute() then
        return fs.absolute(path, base)
    end
    return fs.absolute(path)
end

local function check_config(w2l, type, key)
    local effect  = config[type][key]
    local default = config:raw_default(type, key)
    local global  = config:raw_global(type, key)
    local map     = config:raw_map(type, key)
    local raw
    if map ~= nil then
        raw = map
    elseif global ~= nil then
        raw = global
    else
        raw = default
    end
    if effect == raw then
        return
    end
    if fs.exists(root / 'data' / raw) then
        w2l:failed(lang.script.CONFIG_DIR_VERSION_ERROR:format(type, key))
    else
        w2l:failed(lang.script.CONFIG_DIR_NO_EXISTS:format(type, key))
    end
end

local function normalize_path(w2l, path)
    path = fs.path(path)
    if not path:is_absolute() then
        path = fs.absolute(path, base)
    end
    if path:filename():string() == '.w3x' then
        if check_lni_mark(io.load(path)) then
            return fs.absolute(path:parent_path())
        else
            w2l:failed(lang.script.UNSUPPORTED_LNI_MARK)
            return
        end
    end
    return fs.absolute(path)
end

return function (w2l, mode)
    local setting = { mode = mode }
    local output = output_path(command[3])
    local input, err

    if command[2] then
        input, err = normalize_path(w2l, command[2])
    else
        input, err = get_lni_map()
        if err == 'no lni' then
            w2l:failed(lang.script.NO_LNI)
            return
        elseif err == 'lni mark failed' then
            w2l:failed(lang.script.UNSUPPORTED_LNI_MARK)
            return
        end
    end

    config:open_map(input)
    for k, v in pairs(config.global) do
        setting[k] = v
    end
    if config[setting.mode] then
        for k, v in pairs(config[setting.mode]) do
            setting[k] = v
        end
    end
    setting.input = input
    setting.output = output

    check_config(w2l, 'global' , 'data')
    check_config(w2l, 'global' , 'data_ui')
    check_config(w2l, 'global' , 'data_meta')
    check_config(w2l, 'global' , 'data_wes')
    return setting
end
