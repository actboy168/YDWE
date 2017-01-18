(function()
    local exepath = package.cpath:sub(1, (package.cpath:find(';') or 0)-6)
    package.path = package.path .. ';' .. exepath .. '..\\script\\?.lua'
end)()

require 'filesystem'
local uni = require 'ffi.unicode'
local w2l = require 'w3x2lni'
w2l:initialize()

function message(...)
end

local log = {}
function log.debug(...)
    print(...)
end

local gui = {}
function gui.message(...)
    print(select(2, ...))
end

local slk
local obj
local default
local metadata
local used
local dynamics
local all
local all_chs
local old
local new

local function try_value(t, key)
    if not t then
        return nil, nil, nil
    end
    if key:sub(1, 1) == '_' then
        return nil, nil, nil
    end
    key = key:lower()
    local value = t[key]
    if value then
        if type(value) == 'table' then
            return key, value[1], nil
        else
            return key, value, nil
        end
    end
    local ikey = key .. ':1'
    local value = t[ikey]
    if value then
        return ikey, value, nil
    end
    local pos = key:find("%d+$")
    if not pos then
        return key, nil, nil
    end
    local nkey = key:sub(1, pos-1)
    local ikey = nkey .. ':' .. key:sub(pos)
    local value = t[ikey]
    if value then
        return ikey, value, nil
    end
    local value = t[nkey]
    if not value or type(value) ~= 'table' then
        return nkey, nil, level
    end
    local level = tonumber(key:sub(pos))
    return nkey, value, level
end

local function get_default(t)
    local tp = type(t[1])
    if tp == 'number' then
        if math.type(t[1]) == 'integer' then
            return 0
        else
            return 0.0
        end
    elseif tp == 'string' then
        return ''
    else
        return nil
    end
end

local function get_meta(key, meta1, meta2)
    if key:sub(1, 1) == '_' then
        return nil, nil
    end
    key = key:lower()
    local meta = meta1 and meta1[key] or meta2 and meta2[key]
    if meta then
        if meta['repeat'] then
            return meta, 1
        else
            return meta, nil
        end
    end
    local ikey = key .. ':1'
    local meta = meta1 and meta1[ikey] or meta2 and meta2[ikey]
    if meta then
        return meta, nil
    end
    local pos = key:find("%d+$")
    if not pos then
        return nil, nil
    end
    local nkey = key:sub(1, pos-1)
    local ikey = nkey .. ':' .. key:sub(pos)
    local meta = meta1 and meta1[ikey] or meta2 and meta2[ikey]
    if meta then
        return meta, nil
    end
    local meta = meta1 and meta1[nkey] or meta2 and meta2[nkey]
    if meta and meta['repeat'] then
        return meta, tonumber(key:sub(pos))
    end
    return nil, nil
end

local function to_type(value, tp)
    if tp == 0 then
        value = tonumber(value)
        if not value then
            return nil
        end
        return math.floor(value)
    elseif tp == 1 or tp == 2 then
        return tonumber(value)
    else
        return tostring(value)
    end
end

local chars = {}
local string_list = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'
for i = 1, #string_list do
    chars[i] = string_list:sub(i, i)
end
local function find_id(objs, dynamics, source, tag, ttype)
    local id = dynamics[tag]
    if id then
        local obj = all[id:lower()]
        if not obj then
            return id
        elseif obj._create then
            return nil
        else
            dynamics[tag] = nil
            dynamics[id] = nil
        end
    end
    local first
    if ttype == 'unit' or ttype == 'ability' or ttype == 'buff' then
        first = source:sub(1, 1)
    elseif ttype == 'item' then
        first = 'I'
    elseif ttype == 'destructable' then
        first = 'B'
    elseif ttype == 'doodad' then
        first = 'D'
    elseif ttype == 'upgrade' then
        first = 'R'
    end
    if not all_chs[first] then
        all_chs[first] = {1, 1, 1}
    end
    local chs = all_chs[first]
    while true do
        local id = first .. chars[chs[3]] .. chars[chs[2]] .. chars[chs[1]]
        local lid = id:lower()
        if not all[lid] and not dynamics[id] then
            return id
        end
        for x = 1, 3 do
            chs[x] = chs[x] + 1
            if chars[chs[x]] then
                break
            else
                chs[x] = 1
                if x == 3 then
                    return nil
                end
            end
        end
    end
end

local function create_object(t, ttype, name)
    local mt = {}
    function mt:__index(key)
        local key, value, level = try_value(t, key)
        if not value then
            return ''
        end
        if not level then
            return value
        end
        if level > t._max_level then
            return get_default(value) or ''
        end
        return value[level]
    end
    function mt:__newindex(key, nvalue)
        local objt = obj[ttype][name]
        if not objt or not objt.w2lobject then
            return
        end
        local parent = objt._parent
        local objd = default[ttype][parent]
        local meta, level = get_meta(key, metadata[ttype], objd._code and metadata[objd._code])
        if not meta then
            return
        end
        nvalue = to_type(nvalue, meta.type)
        if not nvalue then
            return
        end
        key = meta.field:lower()
        local dvalue
        if level then
            dvalue = objd[key][level] or (not meta.profile and objd[key][#objd[key]])
        else
            dvalue = objd[key]
        end
        if nvalue == dvalue then
            return
        end
        if meta.type == 3 and #nvalue > 1023 then
            nvalue = nvalue:sub(1, 1023)
        end
        if level then
            if not objt[key] then
                objt[key] = {}
            end
            objt[key][level] = nvalue
        else
            objt[key] = nvalue
        end
        used[ttype] = true
    end
    function mt:__pairs()
        if not t then
            return function() end
        end
        local key
        local level
        return function ()
            if level then
                level = level + 1
                local olevel = level
                if t._max_level <= level then
                    level = nil
                end
                return key .. olevel, t[key][olevel]
            end
            local nkey = next(t, key)
            while true do
                if not nkey then
                    return
                end
                if nkey:sub(1, 1) ~= '_' then
                    break
                end
                nkey = next(t, nkey)
            end
            key = nkey
            if type(t[key]) ~= 'table' then
                return key, t[key]
            end
            if t._max_level > 1 then
                level = 1
            end
            return key .. 1, t[key][1]
        end
    end
    local o = {}
    function o:new(id)
        local objd = default[ttype][name]
        if not objd then
            return create_object(nil, ttype, '')
        end
        if type(id) ~= 'string' then
            return create_object(nil, ttype, '')
        end
        local w2lobject
        if #id == 4 and not id:find('%W') then
            w2lobject = 'static'
            if obj[ttype][id] then
            return create_object(nil, ttype, '')
            end
        else
            w2lobject = 'dynamic|' .. id
            id = find_id(obj[ttype], dynamics[ttype], name, w2lobject, ttype)
            if not id then
            return create_object(nil, ttype, '')
            end
            dynamics[ttype][w2lobject] = id
        end
        local new_obj = {
            _id = id,
            _parent = name,
            _type = ttype,
            _obj = true,
            _code = objd._code,
            _create = true,
            w2lobject = w2lobject,
        }
        obj[ttype][id] = new_obj
        all[id:lower()] = new_obj
        used[ttype] = true
        if old[id] then
            old[id] = nil
        else
            new[id] = new_obj
        end
        return create_object(nil, ttype, id)
    end
    function o:get_id()
        return name
    end
    return setmetatable(o, mt)
end

local function create_proxy(slk, type)
    local t = slk[type]
    local mt = {}
    function mt:__index(key)
        return create_object(t[key], type, key)
    end
    function mt:__newindex()
    end
    function mt:__pairs()
        return function (_, key)
            local nkey = next(t, key)
            if not nkey then
                return
            end
            return nkey, self[nkey]
        end
    end
    return setmetatable({}, mt)
end

local function mark_obj(ttype, objs)
    if not objs then
        return
    end
    for name, obj in pairs(objs) do
        if obj.w2lobject then
            objs[name] = nil
            old[name] = obj
            used[ttype] = true
            local pos = obj.w2lobject:find('|', 1, false)
            if pos then
                local kind = obj.w2lobject:sub(1, pos-1)
                if kind == 'dynamic' then
                    dynamics[ttype][obj.w2lobject] = name
                    dynamics[ttype][name] = obj.w2lobject
                end
            end
        else
            all[name:lower()] = obj
        end
    end
end

local function set_config()
    local config = w2l.config
    -- 转换后的目标格式(lni, obj, slk)
    config.target_format = 'obj'
    -- 是否分析slk文件
    config.read_slk = false
    -- 分析slk时寻找id最优解的次数,0表示无限,寻找次数越多速度越慢
    config.find_id_times = 0
    -- 移除与模板完全相同的数据
    config.remove_same = false
    -- 移除超出等级的数据
    config.remove_exceeds_level = false
    -- 移除只在WE使用的文件
    config.remove_we_only = false
    -- 移除没有引用的对象
    config.remove_unuse_object = false
    -- mdx压缩
    config.mdx_squf = false
    -- 转换为地图还是目录(mpq, dir)
    config.target_storage = 'mpq'

    -- 复制一份物编文件
    config.copy_obj = true
end

local function to_list(tbl)
    local list = {}
    for k in pairs(tbl) do
        list[#list+1] = k
    end
    table.sort(list)
    return list
end

local displaytype = {
    unit = '单位',
    ability = '技能',
    item = '物品',
    buff = '魔法效果',
    upgrade = '科技',
    doodad = '装饰物',
    destructable = '可破坏物',
}

local function get_displayname(o1, o2)
    local name
    if o1._type == 'buff' then
        name = o1.bufftip or o1.editorname or o2.bufftip or o2.editorname
    elseif o1._type == 'upgrade' then
        name = o1.name[1] or o2.name[1]
    else
        name = o1.name or o2.name
    end
    return name:sub(1, 100):gsub('\r\n', ' ')
end

local function create_report()
    local lold = to_list(old)
    local lnew = to_list(new)
    local lines = {}
    if #lold > 0 then
        lines[#lines+1] = ('移除了 %d 个对象'):format(#lold)
        for i = 1, math.min(10, #lold) do
            local o = old[lold[i]]
            lines[#lines+1] = ("[%s][%s] '%s'"):format(displaytype[o._type], get_displayname(o, slk[o._type][o._parent]), o._id)
        end
    end
    if #lnew > 0 then
        if #lines > 0 then
            lines[#lines+1] = ''
        end
        lines[#lines+1] = ('新建了 %d 个对象'):format(#lnew)
        for i = 1, math.min(10, #lnew) do
            local o = new[lnew[i]]
            lines[#lines+1] = ("[%s][%s] '%s'"):format(displaytype[o._type], get_displayname(o, slk[o._type][o._parent]), o._id)
        end
    end
    if #lines > 0 then
        table.insert(lines, 1, '编辑器刚刚帮你修改了物编数据,建议重新打开地图,以便查看变化')
        table.insert(lines, 2, '')
        gui.message(nil, table.concat(lines, '\r\n'))
    end
end

local slk_proxy = {}

function slk_proxy:refresh(mappath)
    if not used then
        return
    end
    create_report()
    local archive = require 'archive'
    local ar = archive(mappath, 'w')
    for _, name in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable'} do
        if used[name] then
            local buf = w2l:backend_obj(name, obj[name])
            log.debug('refresh object: ' .. w2l.info.obj[name])
            ar:save_file(w2l.info.obj[name], buf)
        end
    end
    ar:close()
end

function slk_proxy:initialize(mappath)
    slk = {}
    obj = {}
    used = {}
    all = {}
    dynamics = {}
    old = {}
    new = {}
    all_chs = {}
    default = w2l:get_default()
    metadata = w2l:metadata()
    local archive = require 'archive'
    local ar = archive(mappath)
    set_config()
    w2l:frontend(ar, slk)
    for _, name in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc'} do
        slk_proxy[name] = create_proxy(slk, name)
        dynamics[name] = {}
        obj[name] = slk['copyed_'..name]
        mark_obj(name, obj[name])
    end
    ar:close()
end

local clock = os.clock()
local mappath = fs.path(uni.a2u(arg[1]))
slk_proxy:initialize(mappath)
print('time:', os.clock() - clock)

local clock = os.clock()

print(slk_proxy.unit.h000.Ubertip)
assert(slk_proxy.ability.AHhb.Tip1 == '111,222"333')
assert(slk_proxy.ability.A00A.Cool3 == 65.0)
assert(slk_proxy.ability.Ainf.targs1 == 'air,ground,friend,neutral,self')
assert(slk_proxy.ability.A011.Cool == 1)
assert(slk_proxy.ability.A011.Tip == '激活神圣护甲(|cffffcc00D|r) - [|cffffcc00等级 1|r]')
assert(slk_proxy.ability.A011.Cost1 == 0)
assert(slk_proxy.ability.A011.Cost2 == 25)
assert(slk_proxy.ability.A011.Buttonpos == 1)
assert(slk_proxy.ability.A011.Buttonpos2 == 2)

--for k, v in pairs(slk_proxy.ability.AEim) do
--    print(k, v)
--end

--for id, abil in pairs(slk_proxy.ability) do
--    print(id, abil.DataA1)
--end

slk_proxy.ability.AHds:new 'A123'
slk_proxy.ability.A123.Order = 'tsukiko'
slk_proxy.ability.A123.Dur3 = 123
slk_proxy.ability.A123.levels = '9.9'
slk_proxy.ability.A123.Ubertip2 = ('1'):rep(1022) .. 'ABCDEF'
slk_proxy.ability.A123.Hotkey = 'D'
slk_proxy.ability.A123.Cost = 111
slk_proxy.ability.A123.Cost2 = 133
slk_proxy.ability.A123.Cost3 = 25
slk_proxy.ability.A123.Cost5 = 25
slk_proxy.ability.A123.Cost6 = 123
slk_proxy.ability.A123.Buttonpos = 3
slk_proxy.ability.A123.Buttonpos2 = 1
assert(obj.ability.A123)
assert(obj.ability.A123.order == 'tsukiko')
assert(obj.ability.A123.dur[3] == 123)
assert(obj.ability.A123.levels == 9)
assert(obj.ability.A123.ubertip[2] == ('1'):rep(1022) .. 'A')
assert(obj.ability.A123.hotkey == nil)
assert(obj.ability.A123.cost[1] == 111)
assert(obj.ability.A123.cost[2] == 133)
assert(obj.ability.A123.cost[3] == nil)
assert(obj.ability.A123.cost[5] == nil)
assert(obj.ability.A123.cost[6] == 123)
assert(obj.ability.A123['buttonpos:1'] == 3)
assert(obj.ability.A123['buttonpos:2'] == 1)
assert(slk_proxy.ability.A123.Order == '')

slk_proxy.ability.A234.Order = 'tsukiko'
assert(not obj.ability.A234)

slk_proxy.ability.AHhb.Areaeffectart = '666'
slk_proxy.ability.AHhb.race = 'unknow'
assert(obj.ability.AHhb.areaeffectart == 'xxxxxx')
assert(obj.ability.AHhb.race == nil)
assert(slk_proxy.ability.AHhb.race == 'human')

slk_proxy.ability.AHbz:new 'AHbz'
slk_proxy.ability.AHbz.race = 'unknow'
assert(obj.ability.AHbz.race == 'unknow')
assert(slk_proxy.ability.AHbz.race == 'human')

slk_proxy.ability.AHhb:new 'AHhb'
slk_proxy.ability.AHhb.race = 'unknow'
assert(obj.ability.AHhb.race == nil)
assert(slk_proxy.ability.AHhb.race == 'human')

slk_proxy.ability.AHds:new 'A123'
assert(obj.ability.A123.order == 'tsukiko')

local new_obj = slk_proxy.unit.Hpal:new '测试1'
new_obj.Name = '测试1'
assert(new_obj:get_id() == 'H001')
assert(obj.unit.H001.name == '测试1')

local new_obj = slk_proxy.unit.Hpal:new '测试2'
new_obj.Name = '测试2'
assert(new_obj:get_id() == 'H002')
assert(obj.unit.H002.name == '测试2')

local new_obj = slk_proxy.unit.Hpal:new '测试1'
new_obj.Name = '测试3'
assert(new_obj:get_id() == '')

local clock2 = os.clock()
for i = 1, 1000 do
    local new_obj = slk_proxy.ability.Ainf:new('心灵之火' .. i)
    new_obj.DataB1 = 5 * i
end
print('write: ', os.clock() - clock2)

local output = mappath:parent_path() / (mappath:stem():string() .. '_mod.w3x')
local output2 = mappath:parent_path() / (mappath:stem():string() .. '_mod2.w3x')
fs.copy_file(mappath, output, true)
local clock1 = os.clock()
slk_proxy:refresh(output)
print('copy:', os.clock() - clock1)
print('time:', os.clock() - clock)

local clock = os.clock()
slk_proxy:initialize(output)

local new_obj = slk_proxy.unit.Hpal:new '测试3'
assert(new_obj:get_id() == 'H003')

local new_obj = slk_proxy.unit.Hpal:new '测试2'
assert(new_obj:get_id() == 'H002')

local clock2 = os.clock()
for i = 1, 1000 do
    local new_obj = slk_proxy.ability.Ainf:new('心灵之火' .. i)
    new_obj.DataB1 = 5 * i
end
print('write: ', os.clock() - clock2)

fs.copy_file(output, output2, true)
local clock1 = os.clock()
slk_proxy:refresh(output2)
print('copy:', os.clock() - clock1)
print('time:', os.clock() - clock)

slk_proxy:initialize(output2)
assert(obj.ability.A123 == nil)
assert(obj.ability.A002 == nil)
