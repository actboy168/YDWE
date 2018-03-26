local pairs = pairs
local type = type

local function has_slk(w2l)
    for _, name in ipairs(w2l.info.txt) do
        if w2l:file_load('map', name) then
            return true
        end
    end
    for _, slks in pairs(w2l.info.slk) do
        for _, name in ipairs(slks) do
            if w2l:file_load('map', name) then
                return true
            end
        end
    end
    if w2l:file_load('map', 'war3mapmisc.txt') then
        return true
    end
    return false
end

local function load_slk(w2l)
    if w2l.force_slk then
        w2l.message('-report|9其他', '物编信息不完整,强制读取slk文件')
    end
    if (w2l.force_slk or w2l.config.read_slk) and has_slk(w2l) then
        return w2l:build_slk(true)
    else
        local slk = w2l:get_default(true)
        w2l:frontend_misc(slk)
        return slk
    end
end

local function load_obj(w2l, wts)
    local objs = {}
    local count = 0
    for type, name in pairs(w2l.info.obj) do
        local buf = w2l:file_load('map', name)
        local count = count + 1
        if buf then
            w2l.message('正在转换', name)
            objs[type] = w2l:frontend_obj(type, buf, wts)
            w2l.progress(count / 7)
        end
    end
    return objs
end

local function load_lni(w2l)
    local lnis = {}
    local count = 0
    for type, name in pairs(w2l.info.lni) do
        count = count + 1
        local buf = w2l:file_load('lni', type)
        if buf then
            w2l.message('正在转换', name)
            lnis[type] = w2l:frontend_lni(type, buf, name)
            w2l.progress(count / 7)
        end
    end

    local buf = w2l:file_load('lni', 'txt')
    if buf then
        lnis['txt'] = w2l:parse_lni(buf, 'txt')
    end
    return lnis
end

local function load_w3i(w2l, slk)
    local buf = w2l:file_load('lni', 'w3i')
    if buf then
        return w2l:parse_lni(buf, 'w3i')
    else
        buf = w2l:file_load('map', 'war3map.w3i')
        if buf then
            return w2l:frontend_w3i(buf, slk.wts)
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

local function update_then_merge(w2l, slks, objs, lnis, slk)
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc', 'txt'} do
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
        slk[type] = w2l:frontend_merge(type, data, obj)
        if report then
            for i = 1, 10 do
                local data = report[i]
                if not data then
                    break
                end
                local displayname = get_displayname(slk[type][data[1]])
                w2l.message('-report|6无效的物编数据', ('%s %s %s'):format(displaytype[type], data[1], displayname))
                w2l.message('-tip', ('[%s]: %s'):format(data[2], data[3]))
            end
        end
        if report2 then
            for i = 1, 10 do
                if not report2[i] then
                    break
                end
                w2l.message('-report|6无效的物编数据', report2[i][1])
                if report2[i][2] then
                    w2l.message('-tip', report2[i][2])
                end
            end
        end
    end
end

return function(w2l, slk)
    slk = slk or {}
    w2l.slk = slk
    --读取字符串
    slk.wts = w2l:frontend_wts(w2l:file_load('map', 'war3map.wts'))
    w2l.progress(0.2)

    slk.w3i = load_w3i(w2l, slk)
    update_version(w2l, slk.w3i)

    w2l.message('读取obj...')
    w2l.progress:start(0.4)
    local objs = load_obj(w2l, slk.wts)
    w2l.progress:finish()

    w2l.message('读取lni...')
    w2l.progress:start(0.6)
    local lnis = load_lni(w2l)
    w2l.progress:finish()

    w2l.message('读取slk...')
    w2l.progress:start(0.8)
    local slks = load_slk(w2l)
    w2l.progress:finish()
    
    w2l.message('合并物编数据...')
    w2l.progress:start(1)
    update_then_merge(w2l, slks, objs, lnis, slk)
    w2l.progress:finish()
end
