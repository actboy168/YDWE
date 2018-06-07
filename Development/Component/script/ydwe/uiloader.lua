local event = require 'ev'
local stringify_slk = require 'stringify_slk'
local txt = (require 'w3xparser').txt
local ini = (require 'w3xparser').ini
local slk = (require 'w3xparser').slk
local lni = require 'lni'
local mpqloader = require 'mpqloader'
local load_triggerdata = require 'triggerdata'
local ydwe = fs.ydwe_devpath()
local info = load(io.load(ydwe / 'plugin' / 'w3x2lni' / 'script' / 'core' / 'info.lua'))()
local current_language = (require "i18n").get_language()
local list = {}

local function is_enable_unknownui()
	return true
end

local function load_mpq(filename)
	return mpqloader:load(filename)
end

local function stringify_txt(t)
	local buf = {}
	for id, o in pairs(t) do
		buf[#buf+1] = ('[%s]'):format(id)
		for k, v in pairs(o) do
			for i = 1, #v do
				if v[i]:find(',', 1, true) then
					v[i] = '"' .. v[i] .. '"'
				end
			end
			buf[#buf+1] = ('%s=%s'):format(k, table.concat(v, ','))
		end
	end
	return table.concat(buf, '\r\n')
end

local function stringify_ini(t)
	local buf = {}
	for id, o in pairs(t) do
		buf[#buf+1] = ('[%s]'):format(id)
		for k, v in pairs(o) do
			buf[#buf+1] = ('%s=%s'):format(k, v)
		end
	end
	return table.concat(buf, '\r\n')
end

local data, string

local function load_triggerstrings(name, callback)
	log.trace("virtual_mpq 'triggerstrings'")
	if #list == 0 then
		return nil
	end
	local r = string
	data, string = nil, nil
	return r
end

local function load_worldeditstrings()
	log.trace("virtual_mpq 'worldeditstrings'")
	local t = ini(load_mpq('ui/WorldEditStrings.txt'), 'WorldEditStrings')
	t.WorldEditStrings.WESTRING_APPNAME = t.WorldEditStrings.WESTRING_APPNAME .. ' [ ' .. tostring(ydwe_version) .. ' ]'
	local str = {}
	str[#str+1] = "[WorldEditStrings]"
	for k, v in pairs(t.WorldEditStrings) do
		str[#str+1] = k .. '="' .. v .. '"'
	end
	return table.concat(str, '\n')
end

local function load_worldeditdata()
	log.trace("virtual_mpq 'worldeditdata'")
    local newt = ini(load_mpq('ui/WorldEditData.txt'), 'WorldEditData')
    local ydwewedata = load_mpq('ydwe/WorldEditData.txt')
    if ydwewedata then
        local t = ini(ydwewedata, 'ydwe/WorldEditData')
        if t then
            for id, o in pairs(t) do
                local newo = newt[id]
                if not newo then
                    newo = {}
                    newt[id] = newo
                end
                for k, v in pairs(o) do
                    newo[k] = v
                end
            end
        end
    end
	return stringify_ini(newt)
end

local function initialize()
	list = require 'ui'
    virtual_mpq.watch('UI\\TriggerData.txt',      function ()
        log.trace("virtual_mpq 'triggerdata'")
		data, string = load_triggerdata(list, true)
		return data
    end)
	virtual_mpq.watch('UI\\TriggerStrings.txt',   load_triggerstrings)
	virtual_mpq.watch('UI\\WorldEditStrings.txt', load_worldeditstrings)
	virtual_mpq.watch('UI\\WorldEditData.txt',    load_worldeditdata)
	
	for _, filename in ipairs(info.txt) do
		if info.txt[1] ~= filename then
			virtual_mpq.watch(filename, function () return '' end)
		end
	end
	virtual_mpq.watch(info.txt[1], function ()
		local t = {}
        for _, filename in pairs(info.txt) do
			txt(load_mpq(filename), filename, t)
        end
        local ydwetip = load_mpq('ydwe/ydwetip.txt')
        if ydwetip then
            txt(ydwetip, 'ydwe/ydwetip', t)
        end
        
        local editorsuffix = load_mpq('ydwe/editorsuffix.txt')
        if editorsuffix then
		    local replace = {}
		    txt(editorsuffix, 'ydwe/editorsuffix', replace)
		    for id, o in pairs(replace) do
		    	if not t[id] then
		    		t[id] = o
		    	else
		    		for k, v in pairs(o) do
		    			t[id][k] = v
		    		end
		    	end
            end
        end
		return stringify_txt(t)
	end)
	for _, filenames in pairs(info.slk) do
		for _, filename in ipairs(filenames) do
			if filename ~= 'units\\abilitydata.slk' and filename ~= 'units\\abilitybuffdata.slk' then
				virtual_mpq.watch(filename, function ()
					return load_mpq(filename, filename)
				end)
			end
		end
	end
	virtual_mpq.watch('units\\abilitydata.slk', function ()
		local t = slk(load_mpq('units/abilitydata.slk'), 'abilitydata.slk')
		for _, o in pairs(t) do
			o.useInEditor = 1
		end
		return stringify_slk(t, 'alias')
	end)
	virtual_mpq.watch('units\\abilitybuffdata.slk', function ()
		local t = slk(load_mpq('units/abilitybuffdata.slk'), 'abilitybuffdata.slk')
		local function insert(code, sort, race)
			t[code] = {
				code = code,
				comments = 'YDWE',
				isEffect = 0,
				version = 1,
				useInEditor = 1,
				sort = sort,
				race = race,
				InBeta = 1
			}
		end
		insert('Bdbl', 'hero', 'human') 
		insert('Bdbm', 'hero', 'human')
		insert('BHtb', 'unit', 'other')
		insert('Bsta', 'unit', 'orc')
		insert('Bdbb', 'hero', 'human')
		insert('BIpb', 'item', 'other')
		insert('BIpd', 'item', 'other')
		insert('Btlf', 'unit', 'other')
		return stringify_slk(t, 'alias')
    end)
end

return initialize
