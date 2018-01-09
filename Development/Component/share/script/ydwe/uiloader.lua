require "registry"
local event = require 'ev'
local stringify_slk = require 'stringify_slk'
local txt = (require 'w3xparser').txt
local ini = (require 'w3xparser').ini
local slk = (require 'w3xparser').slk
local lni = require 'lni-c'
local w3x2lni = require 'w3x2lni_in_sandbox'
local w2l = w3x2lni()
local ui = w2l.ui_builder
local storm = require 'ffi.storm'

local ydwe = fs.ydwe_devpath()

local root = ydwe / 'share' / 'ui'
local info = lni(io.load(ydwe / 'plugin' / 'w3x2lni' / 'info.ini'))

local loader = {}

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

function loader:config()
	self.list = {}
	local f, err = io.open(root / 'config', 'r')
	if not f then
		log.error('Open ' .. (root / 'config'):string() .. ' failed.')
		return false
	end
	local enable_ydtrigger = global_config["ThirdPartyPlugin"]["EnableYDTrigger"] ~= "0"
	local enable_japi = is_enable_japi()
	for line in f:lines() do
		if not enable_ydtrigger and (string.trim(line) == 'ydtrigger') then
			-- do nothing
		elseif not enable_japi and (string.trim(line) == 'japi') then
			-- do nothing
		else
			table.insert(self.list, root / string.trim(line))
		end
	end
	f:close()
	if is_enable_unknowui() then
		table.insert(self.list, fs.ydwe_path() / 'share' / 'ui' / 'unknowui')
	end
	return true
end

local state, data, string
function loader:triggerdata(name, callback)
	log.trace("virtual_mpq 'triggerdata'")
	if #self.list == 0 then
		return nil
	end
	state = nil
	for _, path in ipairs(self.list) do
		if fs.exists(path / 'ui') then
			state = ui.merge(state, ui.old_reader(function(filename)
				return io.load(path / 'ui' / filename)
			end))
		else
			state = ui.merge(state, ui.new_reader(function(filename)
				return io.load(path / filename)
			end))
		end
	end
	data, string =  ui.old_writer(state)
	return data
end

function loader:triggerstrings(name, callback)
	log.trace("virtual_mpq 'triggerstrings'")
	if #self.list == 0 then
		return nil
	end
	local r = string
	data, string = nil, nil
	return r
end

function loader:worldeditstrings()
	log.trace("virtual_mpq 'worldeditstrings'")
	local t = ini(io.load(ydwe / 'share' / 'mpq' / 'ui' / 'WorldEditStrings.txt'), 'WorldEditStrings')
	t.WorldEditStrings.WESTRING_APPNAME = t.WorldEditStrings.WESTRING_APPNAME .. ' [ ' .. tostring(ydwe_version) .. ' ]'
	local str = {}
	str[#str+1] = "[WorldEditStrings]"
	for k, v in pairs(t.WorldEditStrings) do
		str[#str+1] = k .. '="' .. v .. '"'
	end
	return table.concat(str, '\n')
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

local mpq_path = fs.path 'share' / 'mpq'
local function load_mpq(filename)
	return io.load(ydwe / mpq_path / filename) or io.load(ydwe / 'share' / 'mpq' / filename)
end

function loader:initialize()
	self:config()
	virtual_mpq.watch('UI\\TriggerData.txt',      function (name) return self:triggerdata() end)
	virtual_mpq.watch('UI\\TriggerStrings.txt',   function (name) return self:triggerstrings() end)
	virtual_mpq.watch('UI\\WorldEditStrings.txt', function (name) return self:worldeditstrings() end)
	
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
		txt(io.load(ydwe / 'share' / 'mpq' / 'ydwe' / 'ydwetip.txt'), 'ydwetip', t)
		
		local replace = {}
		txt(io.load(ydwe / 'share' / 'mpq' / 'ydwe' / 'editorsuffix.txt'), 'editorsuffix', replace)
		for id, o in pairs(replace) do
			if not t[id] then
				t[id] = o
			else
				for k, v in pairs(o) do
					t[id][k] = v
				end
			end
		end
		return stringify_txt(t)
	end)
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
	event.on('virtual_mpq: open path as archive', function(name)
		log.info('OpenPathAsArchive', name)
		mpq_path = fs.path 'share' / 'mpq' / 'Custom_V1'
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
	    	local _, fix = w2l:wtg_reader(wtg, state)
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

uiloader = loader
