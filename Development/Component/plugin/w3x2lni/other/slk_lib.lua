local mt = {}
mt.__index = mt

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
for c in ('0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'):gmatch '.' do
    table.insert(chars, c)
end
function mt:find_id(objs, dynamics, source, tag, ttype)
    local id = dynamics[tag]
    if id then
        local obj = self.all[id:lower()]
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
    if not self.all_chs[first] then
        self.all_chs[first] = {1, 1, 1}
    end
    local chs = self.all_chs[first]
    while true do
        local id = first .. chars[chs[3]] .. chars[chs[2]] .. chars[chs[1]]
        local lid = id:lower()
        if not self.all[lid] and not dynamics[id] then
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

function mt:create_object(objt, ttype, name)
    local session = self
    if not objt and not session.safe_mode then
        return nil
    end
    local mt = {}
    function mt:__index(key)
        local key, value, level = try_value(objt, key)
        local null
        if session.safe_mode then
            null = ''
        end
        if not value then
            return null
        end
        if not level then
            return value
        end
        if level > objt._max_level then
            return get_default(value) or null
        end
        return value[level] or null
    end
    function mt:__newindex(key, nvalue)
        if session.read_only or not objt.w2lobject then
            return
        end
        local parent = objt._parent
        local objd = session.default[ttype][parent]
        local meta, level = get_meta(key, session.metadata[ttype], objd._code and session.metadata[objd._code])
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
        session.used[ttype] = true
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
                meta = get_meta(nkey, session.metadata[ttype], objt._code and session.metadata[objt._code])
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
    if session.read_only then
        return setmetatable(o, mt)
    end
    function o:new(id)
        local objd = session.default[ttype][name]
        if not objd then
            return session:create_object(nil, ttype, '')
        end
        if type(id) ~= 'string' then
            return session:create_object(nil, ttype, '')
        end
        local w2lobject
        if #id == 4 and not id:find('%W') then
            w2lobject = 'static'
            if session.slk[ttype][id] then
                return session:create_object(nil, ttype, '')
            end
        else
            w2lobject = 'dynamic|' .. id
            id = session:find_id(session.slk[ttype], session.dynamics[ttype], name, w2lobject, ttype)
            if not id then
                return session:create_object(nil, ttype, '')
            end
            session.dynamics[ttype][w2lobject] = id
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

        session.slk[ttype][id] = new_obj
        session.all[id:lower()] = new_obj
        session.used[ttype] = true
        if session.old[id] then
            session.old[id] = nil
        else
            session.new[id] = new_obj
        end
        return session:create_object(new_obj, ttype, id)
    end
    function o:get_id()
        return name
    end
    return setmetatable(o, mt)
end

function mt:create_proxy(ttype)
    local t = self.slk[ttype]
    local session = self
    local mt = {}
    function mt:__index(key)
        if type(key) == 'number' then
            local suc, res = pcall(string.pack, '>I4', key)
            if suc then
                key = res
            end
        end
        return session:create_object(t[key], ttype, key)
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

function mt:mark_obj(ttype, objs)
    if not objs then
        return
    end
    for name, obj in pairs(objs) do
        if obj.w2lobject then
            objs[name] = nil
            self.old[name] = obj
            self.used[ttype] = true
            local pos = obj.w2lobject:find('|', 1, false)
            if pos then
                local kind = obj.w2lobject:sub(1, pos-1)
                if kind == 'dynamic' then
                    self.dynamics[ttype][obj.w2lobject] = name
                    self.dynamics[ttype][name] = obj.w2lobject
                end
            end
        else
            self.all[name:lower()] = obj
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

function mt:create_report()
	local lold = to_list(self.old)
	local lnew = to_list(self.new)
	local lines = {}
	if #lold > 0 then
		lines[#lines+1] = ('移除了 %d 个对象'):format(#lold)
		for i = 1, math.min(10, #lold) do
			local o = self.old[lold[i]]
			lines[#lines+1] = ("[%s][%s] '%s'"):format(displaytype[o._type], get_displayname(o, self.slk[o._type][o._parent]), o._id)
		end
	end
	if #lnew > 0 then
		if #lines > 0 then
			lines[#lines+1] = ''
		end
		lines[#lines+1] = ('新建了 %d 个对象'):format(#lnew)
		for i = 1, math.min(10, #lnew) do
			local o = self.new[lnew[i]]
			lines[#lines+1] = ("[%s][%s] '%s'"):format(displaytype[o._type], get_displayname(o, self.slk[o._type][o._parent]), o._id)
		end
    end
    return table.concat(lines, '\n')
end

function mt:refresh(report)
    if not next(self.used) then
        return
    end
    if report then
        report(self:create_report())
    end
    local objs = {}
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable'} do
        if self.used[type] then
            objs[type] = {}
            for name, obj in pairs(self.slk[type]) do
                if obj._parent then
                    objs[type][name] = obj
                end
            end
        end
    end
    self.w2l:backend_cleanobj(objs)
    for type, data in pairs(objs) do
        local buf = self.w2l:backend_obj(type, data)
        self.w2l:map_save(self.w2l.info.obj[type], buf)
    end
end

return function (w2l, read_only, safe_mode)
    local session = setmetatable({
        w2l = w2l,
        read_only = read_only,
        safe_mode = safe_mode,
        slk = {},
        used = {},
        all = {},
        dynamics = {},
        old = {},
        new = {},
        all_chs = {},
    }, mt)

    w2l:frontend(session.slk)
    session.default = w2l:get_default()
    session.metadata = w2l:metadata()

    local slk_proxy = {}
    for _, name in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc'} do
        slk_proxy[name] = session:create_proxy(name)
        session.dynamics[name] = {}
        session:mark_obj(name, session.slk[name])
    end
    if not read_only then
        function slk_proxy:refresh(report)
            session:refresh(report)
        end
    end
    return slk_proxy
end
