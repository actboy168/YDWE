local pairs = pairs
local type = type

local mt = {}
mt.__index = mt

local function copy_table(tbl)
    local new = {}
    for k, v in pairs(tbl) do
        if type(v) == 'table' then
            new[k] = copy_table(v)
        else
            new[k] = v
        end
    end
    return new
end

local function try_value(t, key)
    if not t then
        return nil, nil
    end
    key = key:lower()
    if key == 'code' then
        return t._code, nil
    end
    if key == 'w2lobject' then
        return nil, nil
    end
    local nkey, level = key:match '^(%a+)(%d*)'
    if not nkey then
        return nil, nil
    end
    if t[nkey..'_1'] then
        local values = {}
        local index = 0
        while true do
            index = index + 1
            local k = nkey .. '_' .. index
            local v = t[k]
            if key == k or key == nkey .. ':' .. index then
                return v, nil
            end
            if v then
                values[index] = v
            else
                break
            end
        end
        if key == nkey then
            return table.concat(values, ','), nil
        end
    end

    if t[nkey] then
        local value = t[nkey]
        if type(value) == 'table' then
            if key == nkey then
                return value, 1
            else
                return value, tonumber(level)
            end
        else
            if key == nkey then
                return value, nil
            end
        end
    end

    return t[key], nil
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

local function try_meta(key, meta1, meta2)
    key = key:lower()
    if key == 'w2lobject' then
        return nil, nil, nil
    end
    local nkey, level = key:match '^(%a+)(%d*)'
    if not nkey then
        return nil, nil, nil
    end
    
    local function get_meta(key)
        return meta1 and meta1[key] or meta2 and meta2[key]
    end
    if get_meta(nkey..'_1') then
        if key == nkey then
            return get_meta(nkey..'_1'), nil, 'index'
        end
        local index = 0
        while true do
            index = index + 1
            local k = nkey .. '_' .. index
            local v = get_meta(k)
            if key == k or key == nkey .. ':' .. index then
                return v, nil, nil
            end
            if not v then
                break
            end
        end
    end
    
    if get_meta(nkey) then
        local meta = get_meta(nkey)
        if meta and meta['repeat'] then
            if key == nkey then
                return meta, 1, 'level'
            else
                return meta, tonumber(level), nil
            end
        else
            if key == nkey then
                return meta, nil, nil
            end
        end
    end
    
    return get_meta(key), nil, nil
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

local function fill_data(data, max_level, meta, default)
    if not meta['repeat'] then
        return
    end
    for i = #data + 1, max_level do
        if meta.profile then
            data[i] = default[i] or data[i-1]
        else
            data[i] = default[i] or default[#default]
        end
    end
    for i = max_level + 1, #data do
        data[i] = nil
    end
end

function mt:fill_object(obj, ttype)
    local default = self.default[ttype][obj._parent] or self.default[ttype][obj._id]
    local max_level = obj._max_level
    for key, meta in pairs(self.metadata[ttype]) do
        fill_data(obj[key], max_level, meta, default[key])
    end
    if self.metadata[obj._code] then
        for key, meta in pairs(self.metadata[obj._code]) do
            fill_data(obj[key], max_level, meta, default[key])
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
        local value, level = try_value(objt, key)
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
        if not objt then
            return
        end
        if session.read_only or not objt.w2lobject then
            return
        end
        local parent = objt._parent
        local objd = session.default[ttype][parent]
        local meta, level, list_type = try_meta(key, session.metadata[ttype], objd._code and session.metadata[objd._code])
        if not meta then
            return
        end

        local function write_data(nvalue, level)
            nvalue = to_type(nvalue, meta.type)
            if not nvalue then
                return
            end
            key = meta.field:lower()

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
                if key == session.w2l.info.key.max_level[ttype] then
                    objt._max_level = nvalue
                    session:fill_object(objt, ttype)
                end
            end
        end

        if list_type == 'index' then
            if type(nvalue) ~= 'table' then
                return
            end
            for k, v in pairs(nvalue) do
                self[key..'_'..k] = v
            end
            return
        end

        if list_type == 'level' then
            if type(nvalue) ~= 'table' then
                write_data(nvalue, level)
                return
            end
            local max_level = objt._max_level
            if #nvalue == max_level then
                for i = 1, max_level do
                    write_data(nvalue[i], i)
                end
                return
            end
            if max_level <= 1 then
                return
            end
            local min = nvalue[1]
            local max = nvalue[#nvalue]
            local o = (max - min) / (max_level - 1)
            for i = 1, max_level do
                write_data(min + o * (i - 1), i)
            end
            return
        end

        if type(nvalue) == 'table' then
            return
        end

        write_data(nvalue, level)
    end
    function mt:__pairs()
        if not objt then
            return function() end
        end
        local metadata = session.metadata[ttype]
        local codemetadata = objt._code and session.metadata[objt._code]
        local keys = {}
        local kv = {}
        for k, v in pairs(objt) do
            if k == '_code' then
                keys[#keys+1] = 'code'
                kv.code = v
                goto CONTINUE
            end
            local meta = try_meta(k, metadata, codemetadata)
            if not meta then
                goto CONTINUE
            end
            local key = meta.field
            if type(v) ~= 'table' then
                keys[#keys+1] = key
                kv[key] = v or ''
                goto CONTINUE
            end
            for level = 1, objt._max_level do
                local lkey = key .. level
                keys[#keys+1] = lkey
                kv[lkey] = v[level]
            end
            ::CONTINUE::
        end
        table.sort(keys)
        local i = 0
        return function ()
            i = i + 1
            local k = keys[i]
            return k, kv[k]
        end
    end
    function mt:__call(data)
        if not objt then
            return self
        end
        if session.read_only or not objt.w2lobject then
            return self
        end
        if type(data) ~= 'table' then
            return self
        end
        local level_key = session.w2l.info.key.max_level[ttype]
        if data[level_key] then
            self[level_key] = data[level_key]
            data[level_key] = nil
        end
        for k, v in pairs(data) do
            self[k] = v
        end
        return self
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
            if session.default[ttype][id] or session.slk[ttype][id] then
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
        
        local new_obj = copy_table(objd)
        session:fill_object(new_obj, ttype)
        new_obj._id = id
        new_obj._parent = name
        new_obj._type = ttype
        new_obj._obj = true
        new_obj._code = objd._code
        new_obj._create = true
        new_obj.w2lobject = w2lobject

        session.slk[ttype][id] = new_obj
        session.all[id:lower()] = new_obj
        session.new[ttype][id] = new_obj
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
        local keys = {}
        for k in pairs(t) do
            keys[#keys+1] = k
        end
        table.sort(keys)
        local i = 0
        return function ()
            i = i + 1
            local k = keys[i]
            return k, t[k]
        end
    end
    return setmetatable({}, mt)
end

function mt:mark_obj(ttype, objs)
    if not objs then
        return
    end
    self.new[ttype] = {}
    self.old[ttype] = {}
    for name, obj in pairs(objs) do
        if obj.w2lobject then
            objs[name] = nil
            self.old[ttype][name] = obj
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
    for ttype, data in pairs(tbl) do
        for name, obj in pairs(data) do
            list[#list+1] = obj
        end
    end
    table.sort(list, function (a, b)
        return a._id < b._id
    end)
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
    local lchg = to_list(self.change)
	local lines = {}
	if #lold > 0 then
		lines[#lines+1] = ('移除了 %d 个对象'):format(#lold)
		for i = 1, math.min(10, #lold) do
			local o = lold[i]
			lines[#lines+1] = ("[%s][%s] '%s'"):format(displaytype[o._type], get_displayname(o, self.slk[o._type][o._parent]), o._id)
		end
	end
	if #lnew > 0 then
		if #lines > 0 then
			lines[#lines+1] = ''
		end
		lines[#lines+1] = ('新建了 %d 个对象'):format(#lnew)
		for i = 1, math.min(10, #lnew) do
			local o = lnew[i]
			lines[#lines+1] = ("[%s][%s] '%s'"):format(displaytype[o._type], get_displayname(o, self.slk[o._type][o._parent]), o._id)
		end
    end
	if #lchg > 0 then
		if #lines > 0 then
			lines[#lines+1] = ''
		end
		lines[#lines+1] = ('修改了 %d 个对象'):format(#lchg)
		for i = 1, math.min(10, #lchg) do
			local o = lchg[i]
			lines[#lines+1] = ("[%s][%s] '%s'"):format(displaytype[o._type], get_displayname(o, self.slk[o._type][o._parent]), o._id)
		end
    end
    return table.concat(lines, '\n')
end

local function eq_obj(a, b)
    for k, v in pairs(a) do
        if k == '_create' then
            goto CONTINUE
        end
        if type(v) ~= type(b[k]) then
            return false
        end
        if type(v) == 'table' then
            if not eq_obj(v, b[k]) then
                return false
            end
        else
            if v ~= b[k] then
                return false
            end
        end
        ::CONTINUE::
    end
    return true
end

function mt:update_change(ttype)
    if not self.change[ttype] then
        self.change[ttype] = {}
        local old_map = self.old[ttype]
        local new_map = self.new[ttype]
        for name, obj in pairs(self.new[ttype]) do
            if old_map[name] then
                if not eq_obj(obj, old_map[name]) then
                    self.change[ttype][name] = obj
                end
                old_map[name] = nil
                new_map[name] = nil
            end
        end
    end
    return self.change[ttype]
end

function mt:refresh()
    local objs = {}
    local need_update = {}
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable'} do
        self:update_change(type)
        need_update[type] = next(self.change[type]) or next(self.new[type]) or next(self.old[type])
        if need_update[type] then
            objs[type] = {}
            for name, obj in pairs(self.slk[type]) do
                if obj._parent then
                    objs[type][name] = obj
                end
            end
        end
    end
    self.w2l.config.remove_same = true
    self.w2l:backend_cleanobj(objs)
    for type, data in pairs(objs) do
        if need_update[type] then
            local buf = self.w2l:backend_obj(type, data)
            if buf then
                self.w2l:map_save(self.w2l.info.obj[type], buf)
            else
                self.w2l:map_remove(self.w2l.info.obj[type])
            end
        end
    end
    local report = self:create_report()
    return report
end

return function (w2l, read_only, safe_mode)
    local session = setmetatable({
        w2l = w2l,
        read_only = read_only,
        safe_mode = safe_mode,
        slk = {},
        all = {},
        dynamics = {},
        old = {},
        new = {},
        change = {},
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
        function slk_proxy:refresh()
            return session:refresh()
        end
    end
    return slk_proxy
end
