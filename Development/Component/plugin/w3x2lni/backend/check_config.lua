local config = require 'share.config'
local lang = require 'share.lang'
local w2l

local function check(type, key)
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
    w2l:failed(lang.script.CONFIG_INVALID_DIR:format(type, key, raw))
end

return function (w2l_, input)
    w2l = w2l_
    config:open_map(input)
    check('global' ,'data_war3')
    check('global' ,'data_ui')
    check('global' ,'data_meta')
    check('global' ,'data_wes')
end
