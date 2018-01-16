local w2l
local read_only
local slk
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
    key = key:lower()
    if key == 'code' then
        return 'code', t._code, nil
    end
    if key:sub(1, 1) == '_' then
        return nil, nil, nil
    end
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

local function create_object(objt, ttype, name)
    local mt = {}
    function mt:__index(key)
        local key, value, level = try_value(objt, key)
        if not value then
            return ''
        end
        if not level then
            return value
        end
        if level > objt._max_level then
            return get_default(value) or ''
        end
        return value[level] or ''
    end
    function mt:__newindex(key, nvalue)
        if read_only or not objt.w2lobject then
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
        if not objt then
            return function() end
        end
        local nkey
        local key
        local level
        return function ()
            if level then
                level = level + 1
                local olevel = level
                if objt._max_level <= level then
                    level = nil
                end
                return key .. olevel, objt[nkey][olevel] or ''
            end
            nkey = next(objt, nkey)
            if nkey == '_code' then
                return 'code', objt._code
            end
            local meta
            while true do
                if not nkey then
                    return
                end
                meta = get_meta(nkey, metadata[ttype], objt._code and metadata[objt._code])
                if meta then
                    break
                end
                nkey = next(objt, nkey)
            end
            key = meta.field:gsub(':', '')
            if type(objt[nkey]) ~= 'table' then
                return key, objt[nkey] or ''
            end
            if objt._max_level > 1 then
                level = 1
            end
            return key .. 1, objt[nkey][1] or ''
        end
    end
    local o = {}
    if read_only then
        return setmetatable(o, mt)
    end
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
            if slk[ttype][id] then
                return create_object(nil, ttype, '')
            end
        else
            w2lobject = 'dynamic|' .. id
            id = find_id(slk[ttype], dynamics[ttype], name, w2lobject, ttype)
            if not id then
                return create_object(nil, ttype, '')
            end
            dynamics[ttype][w2lobject] = id
        end
        
        local new_obj = {}
        for k, v in pairs(objd) do
            new_obj[k] = v
        end
        new_obj._id = id
        new_obj._parent = name
        new_obj._type = ttype
        new_obj._obj = true
        new_obj._code = objd._code
        new_obj._create = true
        new_obj.w2lobject = w2lobject

        slk[ttype][id] = new_obj
        all[id:lower()] = new_obj
        used[ttype] = true
        if old[id] then
            old[id] = nil
        else
            new[id] = new_obj
        end
        return create_object(new_obj, ttype, id)
    end
    function o:get_id()
        return name
    end
    return setmetatable(o, mt)
end

local function create_proxy(type)
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

local function to_list(tbl)
	local list = {}
	for k in pairs(tbl) do
		list[#list+1] = k
	end
	table.sort(list)
	return list
end

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

local displaytype = {
    unit = '单位',
    ability = '技能',
    item = '物品',
    buff = '魔法效果',
    upgrade = '科技',
    doodad = '装饰物',
    destructable = '可破坏物',
}

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
    return table.concat(lines, '\n')
end

local slk_proxy = {}

function slk_proxy:refresh(report)
    if not next(used) then
        return
    end
    if report then
        report(create_report())
    end
    local objs = {}
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable'} do
        if used[type] then
            objs[type] = {}
            for name, obj in pairs(slk[type]) do
                if obj._parent then
                    objs[type][name] = obj
                end
            end
        end
    end
    w2l:backend_cleanobj(objs)
    for type, data in pairs(objs) do
        local buf = w2l:backend_obj(type, data)
        w2l:map_save(w2l.info.obj[type], buf)
    end
end

return function (_w2l, _read_only)
    w2l = _w2l
    read_only = _read_only
    slk = {}
    used = {}
    all = {}
    dynamics = {}
    old = {}
    new = {}
    all_chs = {}

    w2l:frontend(slk)
    default = w2l:get_default()
    metadata = w2l:metadata()
    for _, name in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc'} do
        slk_proxy[name] = create_proxy(name)
        dynamics[name] = {}
        mark_obj(name, slk[name])
    end
    return slk_proxy
end
