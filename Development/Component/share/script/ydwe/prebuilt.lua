local mpqloader = require 'mpqloader'

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

local function default2lni(t)
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

local function get_config()
    local config = {}
    -- 转换后的目标格式(lni, obj, slk)
    config.target_format = 'lni'
    -- 是否分析slk文件
    config.read_slk = true
    -- 分析slk时寻找id最优解的次数,0表示无限,寻找次数越多速度越慢
    config.find_id_times = 0
    -- 移除与模板完全相同的数据
    config.remove_same = false
    -- 移除超出等级的数据
    config.remove_exceeds_level = true
    -- 移除只在WE使用的文件
    config.remove_we_only = false
    -- 移除没有引用的对象
    config.remove_unuse_object = false
    -- mdx压缩
    config.mdx_squf = false
    -- 转换为地图还是目录(mpq, dir)
    config.target_storage = 'dir'

    return config
end

return function (w2l, path)
    local config = get_config()
    config.mpq     = mpq
    config.version = w2l.config.version
    config.lang    = w2l.config.lang
    w2l:set_config(config)
    mpqloader:create_directories(path)

	local slk = w2l:build_slk()
    for _, ttype in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc', 'txt'} do
        mpqloader:save(path, ttype .. '.ini', default2lni(slk[ttype]))
    end
end
