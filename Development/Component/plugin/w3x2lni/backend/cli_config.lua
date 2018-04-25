local messager = require 'tool.messager'
local config1, config2 = require 'tool.config' ()
local lang = require 'tool.lang'

local function show_config(section, k, v)
    if v[3] ~= nil then
        messager.raw(('%s %s.%s = %s\r\n'):format(lang.raw.USED_MAP, section, k, tostring(v[3])))
        return
    end
    if v[2] ~= nil then
        messager.raw(('%s %s.%s = %s\r\n'):format(lang.raw.USED_GLOBAL, section, k, tostring(v[2])))
        return
    end
    messager.raw(('%s %s.%s = %s\r\n'):format(lang.raw.USED_DEFAULT, section, k, tostring(v[1])))
end

return function (command)
    if not command[2] then
        messager.raw(lang.raw.CONFIG_DISPLAY .. '\r\n\r\n')
        for section, tbl in pairs(config2) do
            for k, v in pairs(tbl) do
                show_config(section, k, v)
            end
        end
        return
    end

    local request = command[2]
    local section, k = request:match '(%a+)%.([%w_]+)$'
    if section then
        local v = config2[section][k]
        if v then
            messager.raw(v[5])
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
    if section then
        local suc, msg = config2[section][k][4](v)
        if not suc then
            messager.exit('error', msg)
            os.exit(1)
        end
        config1[section][k] = v
        lang:set_lang(config1.global.lang)

        messager.raw(lang.raw.CONFIG_UPDATE .. '\r\n\r\n')
        show_config(section, k, {nil, v})

        if config2[section][k][3] ~= nil then
            messager.raw('\r\n')
            messager.raw(lang.raw.CONFIG_USED_MAP .. '\r\n\r\n')
            show_config(section, k, config2[section][k])
        end
        return
    end
    messager.raw(lang.raw.CONFIG_ERROR)
    messager.raw('\r\n')
end
