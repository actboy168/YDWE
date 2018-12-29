local lang = require 'lang'
local os_clock = os.clock
local w2l

local output = {
    unit    = 'units\\campaignunitstrings.txt',
    ability = 'units\\campaignabilitystrings.txt',
    buff    = 'units\\commonabilitystrings.txt',
    upgrade = 'units\\campaignupgradestrings.txt',
    item    = 'units\\itemstrings.txt',
    txt     = 'units\\itemabilitystrings.txt',
}

local function to_lni(w2l, slk)
    --转换物编
    local count = 0
    for ttype, filename in pairs(w2l.info.lni) do
        count = count + 1
        local data = slk[ttype]
        w2l.progress:start(count / 8)
        local content = w2l:backend_lni(ttype, data)
        w2l.progress:finish()
        if content then
            w2l:file_save('table', ttype, content)
        end
    end

    local content = w2l:backend_txtlni(slk['txt'])
    if content then
        w2l:file_save('table', 'txt', content)
    end
end

local function to_obj(w2l, slk)
    --转换物编
    local count = 0
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc'} do
        local filename = w2l.info.obj[type]
        count = count + 1
        local data = slk[type]
        w2l.progress:start(count / 8)
        local content = w2l:backend_obj(type, data, slk.wts)
        w2l.progress:finish()
        if content and #content > 0 then
            w2l:file_save('map', filename, content)
        end
    end

    local content = w2l:backend_txtlni(slk['txt'])
    if content then
        w2l:file_save('table', 'txt', content)
    end
end

local function convert_wtg(w2l)
    local wtg_data, wct_data
    w2l.progress:start(0.1)
    local wtg = w2l:file_load('map', 'war3map.wtg')
    local wct = w2l:file_load('map', 'war3map.wct')
    w2l.progress:finish()
    w2l.progress:start(0.5)
    if wtg and wct then
        if w2l.setting.mode == 'lni' then
            xpcall(function ()
                wtg_data = w2l:frontend_wtg(wtg)
                wct_data = w2l:frontend_wct(wct)
                w2l:file_remove('map', 'war3map.wtg')
                w2l:file_remove('map', 'war3map.wct')
            end, function (msg)
                w2l.messager.report(lang.report.WARN, 2, lang.report.NO_CONVERT_WTG, msg:match('%.lua:%d+: (.*)'))
            end)
        end
    else
        local version = w2l:file_load('w3x2lni', 'version\\lml')
        if version == nil then
            w2l.frontend_lml = w2l.frontend_lml_v0
        elseif version == '1' then
            w2l.frontend_lml = w2l.frontend_lml_v1
        end
        wtg_data, wct_data = w2l:frontend_lml(function (filename)
            local buf = w2l:file_load('trigger', filename)
            if buf then
                w2l:file_remove('trigger', filename)
            end
            return buf
        end)
        w2l:file_remove('w3x2lni', 'version\\lml')
    end
    w2l.progress:finish()
    w2l.progress:start(1)
    local need_convert_wtg = true
    if wtg_data and wct_data and not w2l.setting.remove_we_only then
        if w2l.setting.mode == 'lni' then
            w2l:file_save('w3x2lni', 'version\\lml', '2')
            local files = w2l:backend_lml(wtg_data, wct_data, w2l.slk.wts)
            for filename, buf in pairs(files) do
                w2l:file_save('trigger', filename, buf)
            end
        else
            local wtg_buf, wct_buf
            local suc, err = pcall(function ()
                wtg_buf = w2l:backend_wtg(wtg_data, w2l.slk.wts)
                wct_buf = w2l:backend_wct(wct_data)
            end)
            if suc then
                w2l:file_save('map', 'war3map.wtg', wtg_buf)
                w2l:file_save('map', 'war3map.wct', wct_buf)
                need_convert_wtg = false
            else
                w2l.messager.report(lang.report.ERROR, 1, lang.report.SAVE_WTG_FAILED, err:match('%.lua:%d+: (.*)'))
            end
        end
    end
    w2l.progress:finish()
    if need_convert_wtg then
        w2l:backend_convertwtg(w2l.slk.wts)
    end
end

local displaytype = {
    unit = lang.script.UNIT,
    ability = lang.script.ABILITY,
    item = lang.script.ITEM,
    buff = lang.script.BUFF,
    upgrade = lang.script.UPGRADE,
    doodad = lang.script.DOODAD,
    destructable = lang.script.DESTRUCTABLE,
}

local function get_displayname(o)
    local name
    if o._type == 'buff' then
        name = o.bufftip or o.editorname or ''
    elseif o._type == 'upgrade' then
        name = o.name[1] or ''
    elseif o._type == 'doodad' or o._type == 'destructable' then
        name = w2l:get_editstring(o.name or '')
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
    w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, ('%s \'%s\' %s'):format(displaytype[type], get_displayname(o)), o._mark and format_marktip(slk, o._mark)) 
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
        misc = {},
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
        w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, lang.report.SIMPLIFIED_OBJECT:format(unuse_origin + unuse_custom, total_origin + total_custom))
    end
    if total_origin - unuse_origin > 0 then
        w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, '---------------------------------------------------------------------')
        w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, lang.report.RETAIN_DEFAULT_OBJECT:format(total_origin - unuse_origin, total_origin))
        w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, '---------------------------------------------------------------------')
        report_list(slk, origin_list, 'unit', 10)
        report_list(slk, origin_list, 'ability', 10)
        report_list(slk, origin_list, 'item', 10)
        report_list(slk, origin_list, 'buff', 10)
        report_list(slk, origin_list, 'upgrade', 3)
        report_list(slk, origin_list, 'destructable', 3)
        report_list(slk, origin_list, 'doodad', 3)
    end
    if unuse_custom > 0 then
        w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, '---------------------------------------------------------------------')
        w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, lang.report.SIMPLIFIED_CUSTOM_OBJECT:format(unuse_custom, total_custom))
        w2l.messager.report(lang.report.REMOVE_UNUSED_OBJECT, 4, '---------------------------------------------------------------------')
        report_list(slk, custom_list, 'unit', 10)
        report_list(slk, custom_list, 'ability', 10)
        report_list(slk, custom_list, 'item', 10)
        report_list(slk, custom_list, 'buff', 10)
        report_list(slk, custom_list, 'upgrade', 3)
        report_list(slk, custom_list, 'destructable', 3)
        report_list(slk, custom_list, 'doodad', 3)
    end
end

local function to_slk(w2l, slk)
    local report = { n = 0 }
    local object = {}
    local slk_list = {'ability', 'buff', 'unit', 'item', 'upgrade', 'destructable'}
    if w2l.setting.slk_doodad then
        slk_list[#slk_list+1] = 'doodad'
    end
    for _, type in ipairs(slk_list) do
        local data = slk[type]
        object[type] = {}
        for id, obj in pairs(data) do
            if obj._keep_obj and not data[obj._parent]._mark then
                data[obj._parent]._mark = obj._mark
            end
        end
        for _, name in ipairs(w2l.info.slk[type]) do
            w2l:file_save('map', name, w2l:backend_slk(type, name, data, report, object[type], slk))
        end
    end

    for _, filename in ipairs(w2l.info.txt) do
        w2l:file_save('map', filename, '')
    end
    local txt = w2l:backend_txt(slk, report, object)
    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade'} do
        w2l:file_save('map', output[type], txt[type])
    end

    for _, type in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'destructable', 'doodad', 'misc'} do
        local data = object[type] or slk[type]
        local content = w2l:backend_obj(type, data, slk.wts)
        if content and #content > 0 then
            w2l:file_save('map', w2l.info.obj[type], content)
        end
    end

    local content = w2l:backend_extra_txt(slk['txt'])
    if content then
        w2l:file_save('map', output['txt'], content)
    end

    if report.n > 0 then
        w2l.messager.report(lang.report.NO_SLK_DATA, 3, 'TOTAL:' .. report.n)
        for tip, list in pairs(report) do
            if #tip > 1 then
                local n = 0
                w2l.messager.report(lang.report.NO_SLK_DATA, 3, '--------------------------------------------')
                w2l.messager.report(lang.report.NO_SLK_DATA, 3, tip)
                w2l.messager.report(lang.report.NO_SLK_DATA, 3, '--------------------------------------------')
                for _, msg in pairs(list) do
                    w2l.messager.report(lang.report.NO_SLK_DATA, 3, msg[1], msg[2])
                    n = n + 1
                    if n > 20 then
                        break
                    end
                end
            end
        end
    end
end

local function clean_file(w2l, slk)
    if w2l.force_slk or w2l.setting.read_slk then
        for _, filename in pairs(w2l.info.txt) do
            w2l:file_remove('map', filename)
        end
        for type, names in pairs(w2l.info.slk) do
            for _, filename in pairs(names) do
                w2l:file_remove('map', filename)
            end
        end
    end
    for _, filename in pairs(w2l.info.obj) do
        w2l:file_remove('map', filename)
    end
    for ttype, filename in pairs(w2l.info.lni) do
        w2l:file_remove('table', ttype)
    end
    w2l:file_remove('table', 'txt')
    w2l:file_remove('table', 'w3i')
    w2l:file_remove('table', 'doo')
end

return function (w2l_, slk)
    w2l = w2l_
    slk = slk or w2l.slk or {}
    w2l.slk = slk
    clean_file(w2l, slk)
    if slk.w3i then
        if w2l.setting.mode == 'lni' then
            w2l:file_save('table', 'w3i', w2l:backend_w3i2lni(slk.w3i), slk.wts)
            w2l:file_remove('map', 'war3map.w3i')
        else
            w2l:file_save('map', 'war3map.w3i', w2l:backend_w3i(slk.w3i, slk.wts))
        end
    end
    w2l.progress(0.1)

    w2l.progress:start(0.1)
    w2l.messager.text(lang.script.CLEAN_SAME_OBJECT)
    w2l:backend_searchparent(slk)
    w2l.progress:finish()

    if w2l.setting.remove_unuse_object then
        w2l.messager.text(lang.script.MARK_UNUSED_OBJECT)
        w2l:backend_mark(slk)
        w2l.progress(0.2)
    end

    if w2l.setting.computed_text then
        w2l.messager.text(lang.script.COMPUTED_TEXT)
        w2l:backend_computed(slk)
        w2l.progress(0.3)
    end

    if w2l.setting.remove_unuse_object then
        w2l.messager.text(lang.script.REMOVE_UNUSED_OBJECT)
        w2l.progress:start(0.5)
        remove_unuse(w2l, slk)
        w2l.progress:finish()
    end

    w2l.progress:start(0.5)
    w2l:backend_cleanobj(slk)
    w2l.progress:finish()
    
    w2l.progress:start(0.7)
    w2l.messager.text(lang.script.CONVERT_OBJ)
    if w2l.setting.mode == 'lni' then
        to_lni(w2l, slk)
    elseif w2l.setting.mode == 'obj' then
        to_obj(w2l, slk)
    elseif w2l.setting.mode == 'slk' then
        to_slk(w2l, slk)
    end
    w2l.progress:finish()

    w2l.progress:start(0.8)
    if not w2l.setting.remove_we_only then
        w2l.messager.text(lang.script.CONVERT_WTG)
        convert_wtg(w2l)
    end
    w2l.progress:finish()

    if w2l.setting.optimize_jass then
        w2l.messager.text(lang.script.OPTIMIZE_JASS)
        w2l:backend_optimizejass()
    end
    w2l.progress(0.9)

    w2l.messager.text(lang.script.CONVERT_OTHER)
    local buf = w2l:file_load('map', 'war3mapskin.txt')
    if buf then
        local skin = w2l:parse_ini(buf)
        w2l:file_save('map', 'war3mapskin.txt', w2l:backend_skin(skin, slk.wts))
    end
    w2l.progress(0.92)

    w2l.messager.text(lang.script.CONVERT_JASS)
    w2l:backend_convertjass(slk.wts)

    w2l.progress(0.95)

    w2l.messager.text(lang.script.REBUILD_WTS)
    local content = w2l:refresh_wts(slk.wts)
    if content and #content > 0 then
        w2l:file_save('map', 'war3map.wts', content)
    else
        w2l:file_remove('map', 'war3map.wts')
    end

    w2l.progress(1)
end
