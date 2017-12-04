local progress = require 'progress'
local loader = require 'loader'
local pairs = pairs
local type = type

local function load_slk(w2l, force_slk)
    if force_slk then
        print('-report|9其他', '物编信息不完整,强制读取slk文件')
    end
    if force_slk or w2l.config.read_slk then
        return w2l:frontend_slk(function(name)
            local buf = loader:map_load(name)
            if buf then
                loader:map_remove(name)
                return buf
            end
            return loader:mpq_load(w2l.agent .. '\\' .. name) or loader:mpq_load(w2l.mpq .. '\\' .. name)
        end)
    else
        return w2l:get_default(true)
    end
end

local function load_obj(w2l, wts)
    local objs = {}
    local force_slk
    local count = 0
    for type, name in pairs(w2l.info.obj) do
        local buf = loader:map_load(name)
        local force
        local count = count + 1
        if buf then
            print('正在转换', name)
            objs[type], force = w2l:frontend_obj(type, buf, wts)
            progress(count / 7)
            if force then
                force_slk = true
            end
            loader:map_remove(name)
        end
    end
    return objs, force_slk
end

local function load_lni(w2l)
    local lnis = {}
    if not w2l.config.read_lni then
        return lnis
    end
    local count = 0
    for type, name in pairs(w2l.info.lni) do
        count = count + 1
        local buf = loader:map_load(name)
        if buf then
            print('正在转换', name)
            lnis[type] = w2l:frontend_lni(type, buf, name)
            progress(count / 7)
            loader:map_remove(name)
        end
    end

    local buf = loader:map_load('war3map.txt.ini')
    if buf then
        lnis['txt'] = w2l:parse_lni(buf, 'war3map.txt.ini')
        loader:map_remove('war3map.txt.ini')
    end
    return lnis
end

local function load_w3i(w2l, slk)
    local buf = loader:map_load 'war3map.w3i.ini'
    if buf and w2l.config.read_lni then
        loader:map_remove('war3map.w3i.ini')
        return w2l:parse_lni(buf, 'war3map.w3i.ini')
    else
        buf = loader:map_load 'war3map.w3i'
        if buf then
            return w2l:read_w3i(buf, slk.wts)
        end
    end
    return nil
end

local function update_version(w2l, w3i)
    if not w3i then
        return
    end
    local melee = w3i['选项']['对战地图']
    local set   = w3i['选项']['使用的游戏数据设置']
    if set == -1 or set == 0 then
        if melee == 0 then
            w2l.config.version = 'Custom'
        elseif melee == 1 then
            w2l.config.version = 'Melee'
        end
    elseif set == 1 then
        w2l.config.version = 'Custom'
    elseif set == 2 then
        w2l.config.version = 'Melee'
    end
    w2l:set_config(w2l.config)
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

local function get_displayname(o)
    local name
    if o._type == 'buff' then
        name = o.bufftip or o.editorname
    elseif o._type == 'upgrade' then
        name = o.name[1]
    else
        name = o.name
    end
    return (name:sub(1, 100):gsub('\r\n', ' '))
end

local function copy_obj(objs)
    local new_objs = {}
    for name, obj in pairs(objs) do
        local new_obj = {}
        new_objs[name] = new_obj
        for key, value in pairs(obj) do
            if type(value) == 'table' then
                local new_value = {}
                new_obj[key] = new_value
                for k, v in pairs(value) do
                    new_value[k] = v
                end
            else
                new_obj[key] = value
            end
        end
    end
    return new_objs
end

local function update_then_merge(w2l, slks, objs, lnis, slk)
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'txt'} do
        local report, report2
        local data = slks[type]
        local obj = objs[type]
        if obj then
            report, report2 = w2l:frontend_updateobj(type, obj, data)
        else
            obj = {}
        end
        if lnis[type] then
            w2l:frontend_updatelni(type, lnis[type], data)
            for k, v in pairs(lnis[type]) do
                obj[k] = v
            end
        end
        if w2l.config.copy_obj then
            slk['copyed_'..type] = copy_obj(obj)
        end
        slk[type] = w2l:frontend_merge(type, data, obj)
        if report then
            for i = 1, 10 do
                local data = report[i]
                if not data then
                    break
                end
                local displayname = get_displayname(slk[type][data[1]])
                print('-report|6无效的物编数据', ('%s %s %s'):format(displaytype[type], data[1], displayname))
                print('-tip', ('[%s]: %s'):format(data[2], data[3]))
            end
        end
        if report2 then
            for i = 1, 10 do
                if not report2[i] then
                    break
                end
                print('-report|6无效的物编数据', report2[i][1])
                if report2[i][2] then
                    print('-tip', report2[i][2])
                end
            end
        end
    end
end

return function(w2l, slk)
    --读取字符串
    slk.wts = w2l:frontend_wts(loader:map_load('war3map.wts'))
    progress(0.2)

    slk.w3i = load_w3i(w2l, slk)
    update_version(w2l, slk.w3i)

    print('读取obj...')
    progress:start(0.4)
    local objs, force_slk1 = load_obj(w2l, slk.wts)
    progress:finish()

    print('读取lni...')
    progress:start(0.6)
    local lnis, force_slk2 = load_lni(w2l)
    progress:finish()

    print('读取slk...')
    progress:start(0.8)
    local slks = load_slk(w2l, force_slk1 or force_slk2)
    progress:finish()
    
    print('合并物编数据...')
    progress:start(1)
    update_then_merge(w2l, slks, objs, lnis, slk)
    progress:finish()
    w2l:frontend_misc(slk)
end
