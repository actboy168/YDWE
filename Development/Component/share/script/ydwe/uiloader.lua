require "registry"
local event = require 'ev'
local stringify_slk = require 'stringify_slk'
local txt = (require 'w3xparser').txt
local ini = (require 'w3xparser').ini
local slk = (require 'w3xparser').slk
local lni = require 'lni-c'
local w3x2lni = require 'w3x2lni_in_sandbox'
local w2l = w3x2lni()
local ui = require 'ui-builder.init'
local storm = require 'ffi.storm'
local mpqloader = require 'mpqloader'
local ydwe = fs.ydwe_devpath()
local info = lni(io.load(ydwe / 'plugin' / 'w3x2lni' / 'info.ini'))
local current_language = (require "i18n").get_language()
local list = {}

local function is_enable_japi()
	local ok, result = pcall(function ()
		local tbl = sys.ini_load(fs.ydwe_path() / 'plugin' / 'warcraft3' / 'config.cfg')
		return tbl['Enable']['yd_jass_api.dll'] ~= '0'
	end)
	if not ok then return true end
	return result
end

local function is_enable_unknowui()
	return true
end

local function load_config()
	list = {}
	local f, err = io.open(ydwe / 'share' / 'ui' / 'config', 'r')
	if not f then
		log.error('Open ' .. (ydwe / 'share' / 'ui' / 'config'):string() .. ' failed.')
		return false
	end
	local enable_ydtrigger = global_config["ThirdPartyPlugin"]["EnableYDTrigger"] ~= "0"
	local enable_japi = is_enable_japi()
    for line in f:lines() do
        line = line:gsub("^%s*(.-)%s*$", "%1") 
		if not enable_ydtrigger and line == 'ydtrigger' then
			-- do nothing
		elseif not enable_japi and line == 'japi' then
			-- do nothing
		else
			table.insert(list, ydwe / 'share' / 'ui' / line)
		end
	end
	f:close()
	if is_enable_unknowui() then
		table.insert(list, fs.ydwe_path() / 'share' / 'ui' / 'unknowui')
	end
	return true
end

local function load_mpq(filename)
	return mpqloader:load(ydwe / 'share' / 'mpq', filename)
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

local state, data, string
local function load_triggerdata(name, callback)
	log.trace("virtual_mpq 'triggerdata'")
	if #list == 0 then
		return nil
    end
	state = nil
	for _, path in ipairs(list) do
		if fs.exists(path / 'ui') then
			state = ui.merge(state, ui.old_reader(function(filename)
				return io.load(path / 'ui' / filename)
			end))
		else
			state = ui.merge(state, ui.new_reader(function(filename)
				return io.load(path / filename)
			end, current_language))
		end
	end
	data, string = ui.old_writer(state)
	return data
end

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
	load_config()
	virtual_mpq.watch('UI\\TriggerData.txt',      load_triggerdata)
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
    if is_enable_unknowui() then
        local ignore_once = nil
        event.on('virtual_mpq: open map', function(mappath)
            if ignore_once == mappath then
                ignore_once = nil
                return
            end
            ignore_once = mappath
            log.info('OpenMap', mappath)
	    	local wtg = storm.load_file('war3map.wtg')
	    	if not wtg then
	    		return
	    	end
	    	if w2l:wtg_checker(wtg, state) then
	    		return
	    	end
            if not gui.yesno_message(nil, LNG.PARSE_UNKNOWN_UI) then
                return
            end
	    	local _, fix = w2l:frontend_wtg(wtg, state)
            local bufs = {ui.new_writer(fix)}
            local dir = fs.ydwe_path() / 'share' / 'ui' / 'unknowui'
	    	fs.create_directories(dir)
	    	io.save(dir / 'define.txt',    bufs[1])
	    	io.save(dir / 'event.txt',     bufs[2])
	    	io.save(dir / 'condition.txt', bufs[3])
	    	io.save(dir / 'action.txt',    bufs[4])
            io.save(dir / 'call.txt',      bufs[5])
            sys.reboot(mappath)
        end)
    end
end

return initialize
