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

return function (_w2l, load_map)
    w2l = _w2l
    
    local slk = w2l.parse_slk
    
	local hook
	function w2l:parse_slk(buf)
		if hook then
			local r = slk(self, buf)
			hook(r)
			hook = nil
			return r
		end
		return slk(self, buf)
    end
    
	local result = w2l:frontend_slk(function(name)
		if name:lower() == 'units\\abilitybuffdata.slk' then
			function hook(t)
                merge_slk(t, abilitybuffdata)
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

    w2l:frontend_misc(result)

    w2l.parse_slk = slk

    if w2l.prebuilt_save then
        for type, data in pairs(result) do
            w2l:prebuilt_save(type .. '.ini', writer(data))
        end
    end

	return result
end
