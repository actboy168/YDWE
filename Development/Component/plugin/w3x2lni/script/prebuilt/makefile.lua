local lang = require 'share.lang'
local config = require 'share.config'
local root = require 'backend.w2l_path'

local function sortpairs(t)
    local sort = {}
    for k, v in pairs(t) do
        sort[#sort+1] = {k, v}
    end
    table.sort(sort, function (a, b)
        return a[1] < b[1]
    end)
    local n = 1
    return function()
        local v = sort[n]
        if not v then
            return
        end
        n = n + 1
        return v[1], v[2]
    end
end

local function format_value(value)
    local tp = type(value)
    if tp == 'boolean' then
        return tostring(value)
    end
    if tp == 'number' then
        return tostring(value)
    end
    if tp == 'string' then
        return ('%q'):format(value)
    end
end

local function maxindex(t)
    local i = 0
    for k in pairs(t) do
        i = math.max(i, k)
    end
    return i
end

local function write_data(f, k, v)
    if k:find '[^%w_]' then
        k = ('%q'):format(k)
    end
    if type(v) == 'table' then
        local l = {}
        for i = 1, maxindex(v) do
            l[i] = format_value(v[i]) or 'nil'
        end
        f[#f+1] = ('%s={%s}'):format(k, table.concat(l, ','))
    else
        f[#f+1] = ('%s=%s'):format(k, format_value(v))
    end
end

function writer(t)
    local f = {}
    for i, o in sortpairs(t) do
        f[#f+1] = ('[%s]'):format(i)
        for k, v in sortpairs(o) do
            write_data(f, k, v)
        end
        f[#f+1] = ''
    end
    return table.concat(f, '\r\n')
end

return function (w2l, version)
    w2l.messager.text(lang.script.CONVERT_ONE .. version)

    w2l:set_setting
    {
        data      = config.global.data,
        data_ui   = config.global.data_ui,
        data_meta = config.global.data_meta,
        data_wes  = config.global.data_wes,
        version   = version,
    }
    local prebuilt_path = root / 'data' / config.global.data / 'prebuilt' / version
    fs.create_directories(prebuilt_path)

    local slk = w2l:frontend_buildslk()
    for type, data in pairs(slk) do
        io.save(prebuilt_path / (type .. '.ini'), writer(data))
    end
    return slk
end
