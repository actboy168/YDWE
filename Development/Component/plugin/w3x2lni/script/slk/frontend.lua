local progress = require 'progress'

local pairs = pairs
local type = type

local function load_slk(w2l, archive, force_slk)
    if force_slk then
        message('-report|9其他', '物编信息不完整,强制读取slk文件')
    end
    if force_slk or w2l.config.read_slk then
        return w2l:frontend_slk(function(name)
            local buf = archive:get(name)
            if buf then
                archive:set(name, false)
                return buf
            end
            return io.load(w2l.mpq / name)
        end)
    else
        return w2l:get_default(true)
    end
end

local function load_obj(w2l, archive, wts)
    local objs = {}
    local force_slk
    local count = 0
    for type, name in pairs(w2l.info.obj) do
        local buf = archive:get(name)
        local force
        local count = count + 1
        if buf then
            message('正在转换', name)
            objs[type], force = w2l:frontend_obj(type, buf, wts)
            progress(count / 7)
            if force then
                force_slk = true
            end
            archive:set(name, false)
        end
    end
    return objs, force_slk
end

local function load_lni(w2l, archive)
    local lnis = {}
    if not w2l.config.read_lni then
        return lnis
    end
    local count = 0
    for type, name in pairs(w2l.info.lni) do
        count = count + 1
        local buf = archive:get(name)
        if buf then
            message('正在转换', name)
            lnis[type] = w2l:frontend_lni(type, buf)
            progress(count / 7)
            archive:set(name, false)
        end
    end

    local buf = archive:get('war3map.txt.ini')
    if buf then
        lnis['txt'] = w2l:parse_lni(buf, 'txt')
        archive:set('war3map.txt.ini', false)
    end
    return lnis
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
                message('-report|6无效的物编数据', ('%s %s %s'):format(displaytype[type], data[1], displayname))
                message('-tip', ('[%s]: %s'):format(data[2], data[3]))
            end
        end
        if report2 then
            for i = 1, 10 do
                if not report2[i] then
                    break
                end
                message('-report|6无效的物编数据', report2[i][1])
                if report2[i][2] then
                    message('-tip', report2[i][2])
                end
            end
        end
    end
end

return function(w2l, archive, slk)
    --读取字符串
    slk.wts = w2l:frontend_wts(archive:get('war3map.wts'))
    progress(0.2)

    message('读取obj...')
    progress:start(0.4)
    local objs, force_slk1 = load_obj(w2l, archive, slk.wts)
    progress:finish()

    message('读取lni...')
    progress:start(0.6)
    local lnis, force_slk2 = load_lni(w2l, archive)
    progress:finish()

    message('读取slk...')
    progress:start(0.8)
    local slks = load_slk(w2l, archive, force_slk1 or force_slk2)
    progress:finish()
    
    message('合并物编数据...')
    progress:start(1)
    update_then_merge(w2l, slks, objs, lnis, slk)
    progress:finish()
    w2l:frontend_misc(archive, slk)
end
