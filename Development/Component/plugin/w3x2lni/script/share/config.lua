require 'filesystem'
local lni = require 'lni'
local define = require 'share.config_define'
local root = require 'backend.w2l_path'
local default_config = lni.classics(io.load(root / 'script' / 'share' / 'config.ini'), 'script\\share\\config.ini')
local global_config  = lni.classics(io.load(root / 'config.ini'), 'config.ini')
local map_config = {}

local config = {}

local function save()
    local lines = {}
    for name, t in pairs(config) do
        lines[#lines+1] = ('[%s]'):format(name)
        for k, v in pairs(t) do
            if define[name][k][2] ~= nil then
                local value = global_config[name][k]
                if value == nil then
                    value = default_config[name][k]
                end
                local _, _, fmt = define[name][k][1](value)
                lines[#lines+1] = ('%s = %s'):format(k, fmt)
            end
        end
        lines[#lines+1] = ''
    end
    local buf = table.concat(lines, '\r\n')
    io.save(root / 'config.ini', buf)
end

local function proxy(default, global, map, define, table)
    local table = table or {}
    if define._child then
        for _, k in ipairs(define) do
            if not default[k] then
                default[k] = {}
            end
            if not global[k] then
                global[k] = {}
            end
            if not map[k] then
                map[k] = {}
            end
            table[k] = proxy(default[k], global[k], map[k], define[k])
        end
    end
    local list = { default, global, map }
    setmetatable(table, {
        __index = function (_, k)
            if not define[k] then
                return nil
            end
            for i = 3, 1, -1 do
                local lni = list[i]
                if lni[k] ~= nil then
                    local suc, res = define[k][1](lni[k])
                    if suc then
                        return res
                    end
                end
            end
        end,
        __newindex = function (_, k, v)
            local _, v = define[k][1](v)
            global[k] = v
            save()
        end,
        __pairs = function ()
            local i = 0
            return function ()
                i = i + 1
                local k = define[i]
                return k, table[k]
            end
        end,
    })
    return table
end

function config:open_map(path)
    local builder = require 'map-builder'
    local map = builder.load(path)
    if map then
        lni.classics(map:get 'w3x2lni\\config.ini' or '', 'w3x2lni\\config.ini', { map_config })
        map:close()
    end
end

function config:close_map()
    for k in pairs(map_config) do
        map_config[k] = nil
    end
end

function config:raw_default(k1, k2)
    return default_config[k1][k2]
end

function config:raw_global(k1, k2)
    return global_config[k1][k2]
end

function config:raw_map(k1, k2)
    return map_config[k1][k2]
end

function config:define_check(k1, k2, v)
    local definer = define[k1][k2]
    if not definer then
        local lang = require 'share.lang'
        return false, lang.raw.INVALID_CONFIG
    end
    local suc, res, fmt = definer[1](v)
    if suc then
        return suc, res, fmt
    else
        return suc, tostring(res)
    end
end

function config:define_comment(k1, k2)
    local definer = define[k1][k2]
    if not definer then
        local lang = require 'share.lang'
        return false, lang.raw.INVALID_CONFIG
    end
    return tostring(definer[2])
end

function config:define_visible(k1, k2)
    local definer = define[k1][k2]
    if not definer then
        return false
    end
    return definer[2] ~= nil
end

return proxy(default_config, global_config, map_config, define, config)
