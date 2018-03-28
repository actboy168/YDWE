local w2l

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

function writer(t)
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

local abilitybuffdata = {
    {'alias',   'code', 'comments', 'isEffect', 'version', 'useInEditor', 'sort', 'race' , 'InBeta'},
    ['Bdbl'] = {'Bdbl', 'YDWE'    , 0         , 1        , 1            , 'hero', 'human', 1       },
    ['Bdbm'] = {'Bdbm', 'YDWE'    , 0         , 1        , 1            , 'hero', 'human', 1       },
    ['BHtb'] = {'BHtb', 'YDWE'    , 0         , 1        , 1            , 'unit', 'other', 1       },
    ['Bsta'] = {'Bsta', 'YDWE'    , 0         , 1        , 1            , 'unit', 'orc'  , 1       },
    ['Bdbb'] = {'Bdbb', 'YDWE'    , 0         , 1        , 1            , 'hero', 'human', 1       },
    ['BIpb'] = {'BIpb', 'YDWE'    , 0         , 1        , 1            , 'item', 'other', 1       },
    ['BIpd'] = {'BIpd', 'YDWE'    , 0         , 1        , 1            , 'item', 'other', 1       },
    ['Btlf'] = {'Btlf', 'YDWE'    , 0         , 1        , 1            , 'unit', 'other', 1       },
}

local function merge_slk(t, fix)
    for k, v in pairs(fix) do
        if k ~= 1 then
            t[k] = {}
            for i, key in ipairs(fix[1]) do
                if i ~= 1 then
                    t[k][key] = v[i-1]
                end
            end
        end
    end
end

local miscdata = {
    ['Misc'] = {
        ['GoldTextHeight']             = {0.024},
        ['GoldTextVelocity']           = {0, 0.03},
        ['LumberTextHeight']           = {0.024},
        ['LumberTextVelocity']         = {0, 0.03},
        ['BountyTextHeight']           = {0.024},
        ['BountyTextVelocity']         = {0, 0.03},
        ['MissTextHeight']             = {0.024},
        ['MissTextVelocity']           = {0, 0.03},
        ['CriticalStrikeTextHeight']   = {0.024},
        ['CriticalStrikeTextVelocity'] = {0, 0.04},
        ['ShadowStrikeTextHeight']     = {0.024},
        ['ShadowStrikeTextVelocity']   = {0, 0.04},
        ['ManaBurnTextHeight']         = {0.024},
        ['ManaBurnTextVelocity']       = {0, 0.04},
        ['BashTextVelocity']           = {0, 0.04},
    },
    ['Terrain'] = {
        ['MaxSlope']                   = {90},
        ['MaxHeight']                  = {1920},
        ['MinHeight']                  = {-1920},
    },
    ['FontHeights'] = {
        ['ToolTipName']                = {0.011},
        ['ToolTipDesc']                = {0.011},
        ['ToolTipCost']                = {0.011},
        ['ChatEditBar']                = {0.013},
        ['CommandButtonNumber']        = {0.009},
        ['WorldFrameMessage']          = {0.015},
        ['WorldFrameTopMessage']       = {0.024},
        ['WorldFrameUnitMessage']      = {0.015},
        ['WorldFrameChatMessage']      = {0.013},
        ['Inventory']                  = {0.011},
        ['LeaderBoard']                = {0.007},
        ['PortraitStats']              = {0.011},
        ['UnitTipPlayerName']          = {0.011},
        ['UnitTipDesc']                = {0.011},
        ['ScoreScreenNormal']          = {0.011},
        ['ScoreScreenLarge']           = {0.011},
        ['ScoreScreenTeam']            = {0.009},
    },
}

local function merge_txt(t, fix)
    for name, data in pairs(fix) do
        name = name:lower()
        if not t[name] then
            t[name] = {}
        end
        for k, v in pairs(data) do
            k = k:lower()
            t[name][k] = v
        end
    end
end

return function (_w2l, load_map)
    w2l = _w2l
    
    local slk = w2l.parse_slk
    local txt = w2l.parse_txt
    
    local hook
    function w2l:parse_slk(...)
        if hook then
            local r = slk(self, ...)
            hook(r)
            hook = nil
            return r
        end
        return slk(self, ...)
    end

    function w2l:parse_txt(...)
        if hook then
            local r = txt(self, ...)
            hook(r)
            hook = nil
            return r
        end
        return txt(self, ...)
    end
    
    local result = w2l:frontend_slk(function(name)
        if name:lower() == 'units\\abilitybuffdata.slk' then
            function hook(t)
                merge_slk(t, abilitybuffdata)
            end
        end
        if name:lower() == 'ui\\miscdata.txt' then
            function hook(t)
                merge_txt(t, miscdata)
            end
        end
        if load_map then
            local buf = w2l:file_load('map', name)
            if buf then
                return buf
            end
        end
        return w2l:mpq_load(name)
    end)

    w2l.parse_slk = slk
    w2l.parse_txt = txt

    if w2l.prebuilt_save then
        for type, data in pairs(result) do
            w2l:prebuilt_save(type .. '.ini', writer(data))
        end
    end

    return result
end
