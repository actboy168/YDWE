local progress = require 'progress'

local os_clock = os.clock

local output = {
    unit    = 'units\\campaignunitstrings.txt',
    ability = 'units\\campaignabilitystrings.txt',
    buff    = 'units\\commonabilitystrings.txt',
    upgrade = 'units\\campaignupgradestrings.txt',
    item    = 'units\\itemstrings.txt',
    txt     = 'units\\itemabilitystrings.txt',
}

local function to_lni(w2l, archive, slk)
    --转换物编
    local count = 0
    for ttype, filename in pairs(w2l.info.lni) do
        count = count + 1
        local data = slk[ttype]
        progress:start(count / 7)
        local content = w2l:backend_lni(ttype, data)
        progress:finish()
        if content then
            archive:set(filename, content)
        end
    end

    local content = w2l:backend_txtlni(slk['txt'])
    if content then
        archive:set('war3map.txt.ini', content)
    end
end

local function to_obj(w2l, archive, slk)
    --转换物编
    local count = 0
    for type, filename in pairs(w2l.info.obj) do
        count = count + 1
        local data = slk[type]
        progress:start(count / 7)
        local content = w2l:backend_obj(type, data, slk.wts)
        progress:finish()
        if content then
            archive:set(filename, content)
        end
    end

    local content = w2l:backend_txtlni(slk['txt'])
    if content then
        archive:set('war3map.txt.ini', content)
    end
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
        name = o.bufftip or o.editorname or ''
    elseif o._type == 'upgrade' then
        name = o.name[1] or ''
    else
        name = o.name or ''
    end
    return o._id, (name:sub(1, 100):gsub('\r\n', ' '))
end

local function get_displayname_by_id(slk, id)
    local o = slk.ability[id]
           or slk.unit[id]
           or slk.buff[id]
           or slk.item[id]
           or slk.destructable[id]
           or slk.doodad[id]
           or slk.upgrade[id]
    if not o then
        return id, '<unknown>'
    end
    return get_displayname(o)
end

local function format_marktip(slk, marktip)
    return marktip[2]:format(get_displayname_by_id(slk, marktip[1]))
end

local function report_object(slk, type, o)
    message('-report|4简化', displaytype[type], get_displayname(o))
    if o._mark then
        message('-tip', format_marktip(slk, o._mark))
    end
end

local function report_list(slk, list, type, n)
    list = list[type]
    for i = 1, math.min(n, #list) do
        report_object(slk, type, list[i])
    end
end

local function remove_unuse(w2l, slk)
    local custom_list = {
        ability = {},
        unit = {},
        item = {},
        buff = {},
        upgrade = {},
        doodad = {},
        destructable = {},
    }
    local origin_list = {
        ability = {},
        unit = {},
        item = {},
        buff = {},
        upgrade = {},
        doodad = {},
        destructable = {},
    }
    local mustuse = {
        ability = {},
        unit = {},
        item = {},
        buff = {},
        upgrade = {},
        doodad = {},
        destructable = {},
    }
    for type, list in pairs(slk.mustuse) do
        for _, id in ipairs(list) do
            mustuse[type][id] = true
        end
    end
    
    local total_custom = 0
    local total_origin = 0
    local unuse_custom = 0
    local unuse_origin = 0
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable'} do
        local default = w2l:get_default()[type]
        for name, obj in pairs(slk[type]) do
            if obj._obj or not default[name] then
                total_custom = total_custom + 1
                if not obj._mark then
                    unuse_custom = unuse_custom + 1
                    custom_list[type][#custom_list[type]+1] = obj
                end
            else
                total_origin = total_origin + 1
                if not obj._mark then
                    unuse_origin = unuse_origin + 1
                else
                    if not mustuse[type][name] then
                        origin_list[type][#origin_list[type]+1] = obj
                    end
                end
            end
        end
    end

    if unuse_origin + unuse_custom > 0 then
        message('-report|4简化', ('简化掉的对象数: %d/%d'):format(unuse_origin + unuse_custom, total_origin + total_custom))
    end
    if total_origin - unuse_origin > 0 then
        message('-report|4简化', ('保留的默认对象数: %d/%d'):format(total_origin - unuse_origin, total_origin))
        report_list(slk, origin_list, 'unit', 10)
        report_list(slk, origin_list, 'ability', 10)
        report_list(slk, origin_list, 'item', 10)
        report_list(slk, origin_list, 'buff', 10)
        report_list(slk, origin_list, 'upgrade', 3)
        report_list(slk, origin_list, 'destructable', 3)
        report_list(slk, origin_list, 'doodad', 3)
    end
    if unuse_custom > 0 then
        message('-report|4简化', ('简化掉的自定义对象数: %d/%d'):format(unuse_custom, total_custom))
        report_list(slk, custom_list, 'unit', 10)
        report_list(slk, custom_list, 'ability', 10)
        report_list(slk, custom_list, 'item', 10)
        report_list(slk, custom_list, 'buff', 10)
        report_list(slk, custom_list, 'upgrade', 3)
        report_list(slk, custom_list, 'destructable', 3)
        report_list(slk, custom_list, 'doodad', 3)
    end
end

local function to_slk(w2l, archive, slk)
    local report = { n = 0 }
    local object = {}
    local slk_list = {'ability', 'buff', 'unit', 'item', 'upgrade', 'destructable'}
    if w2l.config.slk_doodad then
        slk_list[#slk_list+1] = 'doodad'
    end
    for _, type in ipairs(slk_list) do
        local data = slk[type]
        object[type] = {}
        for _, slk in ipairs(w2l.info.slk[type]) do
            archive:set(slk, w2l:backend_slk(type, slk, data, report, object[type]))
        end
    end

    for _, filename in ipairs(w2l.info.txt) do
        archive:set(filename, '')
    end
    local txt = w2l:backend_txt(slk, report, object)
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade'} do
        archive:set(output[type], txt[type])
    end

    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'destructable', 'doodad'} do
        local data = object[type] or slk[type]
        local content = w2l:backend_obj(type, data, slk.wts)
        if content then
            archive:set(w2l.info.obj[type], content)
        end
    end

    local content = w2l:backend_extra_txt(slk['txt'])
    if content then
        archive:set(output['txt'], content)
    end

    if report.n > 0 then
        local index = 1
        message('-report|3没有SLK化的数据', ('合计: %d'):format(report.n))
        for tip, list in pairs(report) do
            if #tip > 1 then
                local n = 0
                message('-report|3没有SLK化的数据', ('%d.%s'):format(index, tip))
                index = index + 1
                for _, msg in pairs(list) do
                    message('-report|3没有SLK化的数据', msg[1])
                    message('-tip', msg[2])
                    n = n + 1
                    if n > 20 then
                        break
                    end
                end
            end
        end
    end
end

local function load_w3i(w2l, archive, slk)
    local buf = archive:get 'war3map.w3i.ini'
    if buf then
        slk.w3i = w2l:parse_lni(buf)
        archive:set('war3map.w3i.ini', false)
    else
        buf = archive:get 'war3map.w3i'
        if buf then
            slk.w3i = w2l:read_w3i(buf, slk.wts)
        end
    end
    if not slk.w3i then
        return
    end
    if slk.w3i['选项']['使用的游戏数据设置'] == 1 then
        message('-report|2警告', '不支持的游戏数据设置"自定义"')
        message('-tip', '只支持"默认(1.07)",数据可能会出现差异')
    end
    if slk.w3i['选项']['使用的游戏数据设置'] == 2 then
        message('-report|2警告', '不支持的游戏数据设置"对战(最新版本)"')
        message('-tip', '只支持"默认(1.07)",数据可能会出现差异')
    end
end

return function (w2l, archive, slk)
    load_w3i(w2l, archive, slk)
    if slk.w3i then
        if w2l.config.target_format == 'lni' then
            archive:set('war3map.w3i.ini', w2l:w3i2lni(slk.w3i), slk.wts)
            archive:set('war3map.w3i', false)
        else
            archive:set('war3map.w3i', w2l:lni2w3i(slk.w3i, slk.wts))
        end
    end
    progress(0.1)

    progress:start(0.1)
    message('清理数据...')
    w2l:backend_searchparent(slk)
    progress:finish()

    if w2l.config.remove_unuse_object then
        message('标记简化对象...')
        w2l:backend_mark(archive, slk)
        progress(0.2)
    end

    if w2l.config.target_format == 'slk' then
        message('计算描述中的公式...')
        w2l:backend_computed(slk)
        progress(0.3)
    end

    if w2l.config.remove_unuse_object then
        message('移除简化对象...')
        progress:start(0.5)
        remove_unuse(w2l, slk)
        progress:finish()
    end

    progress:start(0.7)
    w2l:backend_cleanobj(slk)
    progress:finish()
    
    progress:start(0.9)
    message('转换物编文件...')
    if w2l.config.target_format == 'lni' then
        to_lni(w2l, archive, slk)
    elseif w2l.config.target_format == 'obj' then
        to_obj(w2l, archive, slk)
    elseif w2l.config.target_format == 'slk' then
        to_slk(w2l, archive, slk)
    end
    progress:finish()

    message('转换脚本...')
    w2l:backend_convertjass(archive, slk.wts)
    w2l:backend_convertwtg(archive, slk.wts)
    progress(0.92)

    message('转换其他文件...')
    archive:set('war3mapmisc.txt', w2l:backend_misc(slk.misc, slk.txt, slk.wts))
    progress(0.93)

    local buf = archive:get 'war3mapskin.txt'
    if buf then
        local skin = w2l:parse_ini(buf)
        archive:set('war3mapskin.txt', w2l:backend_skin(skin, slk.wts))
    end
    progress(0.94)

    if w2l.config.target_format == 'lni' then
        local buf = archive:get 'war3map.imp'
        if buf then
            archive:set('war3map.imp', false)
            archive:set('war3map.imp.ini', w2l:backend_imp(archive, buf))
        end
    end

    message('重新生成字符串...')
    local content = w2l:refresh_wts(slk.wts)
    if #content > 0 then
        archive:set('war3map.wts', content)
    else
        archive:set('war3map.wts', false)
    end
    progress(0.95)

    if w2l.config.optimize_jass then
        message('优化脚本...')
        w2l:backend_optimizejass(archive)
    end
    progress(1)
end
