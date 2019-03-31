local messager = require 'share.messager'
local config = require 'share.config'
local lang = require 'share.lang'
local get_lni_map = require 'backend.get_lni_map'

local function show_config(section, k, v)
    local map = config:raw_map(section, k)
    if map ~= nil then
        messager.raw(('%s %s.%s = %s\r\n'):format(lang.raw.USED_MAP, section, k, map))
        return
    end
    local global = config:raw_global(section, k)
    if global ~= nil then
        messager.raw(('%s %s.%s = %s\r\n'):format(lang.raw.USED_GLOBAL, section, k, global))
        return
    end
    local default = config:raw_default(section, k)
    messager.raw(('%s %s.%s = %s\r\n'):format(lang.raw.USED_DEFAULT, section, k, default))
end

return function (command)
    config:open_map(get_lni_map())
    if not command[2] then
        messager.raw(lang.raw.CONFIG_DISPLAY .. '\r\n\r\n')
        for section, tbl in pairs(config) do
            for k, v in pairs(tbl) do
                if config:define_visible(section, k) then
                    show_config(section, k, v)
                end
            end
        end
        return
    end

    local request = command[2]
    local section, k = request:match '(%a+)%.([%w_]+)$'
    if section and config:define_visible(section, k) then
        local v = config:define_comment(section, k)
        if v then
            messager.raw(v)
            messager.raw('\r\n\r\n')
            messager.raw(lang.raw.CONFIG_DISPLAY .. '\r\n\r\n')
            show_config(section, k, v)
        else
            messager.raw(lang.raw.CONFIG_ERROR)
            messager.raw('\r\n')
        end
        return
    end
    local section, k, v = request:match '(%a+)%.([%w_]+)%=(.*)$'
    if section and config:define_visible(section, k) then
        local suc, msg = config:define_check(section, k, v)
        if not suc then
            messager.exit('error', msg)
            os.exit(1)
        end
        config[section][k] = v
        lang:set_lang(config.global.lang)

        messager.raw(lang.raw.CONFIG_UPDATE .. '\r\n\r\n')
        show_config(section, k, {nil, v})

        if config:raw_map(section, k) ~= nil then
            messager.raw('\r\n')
            messager.raw(lang.raw.CONFIG_USED_MAP .. '\r\n\r\n')
            show_config(section, k, config:raw_map(section, k))
        end
        return
    end
    messager.raw(lang.raw.CONFIG_ERROR)
    messager.raw('\r\n')
end
