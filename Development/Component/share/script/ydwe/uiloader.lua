require "registry"
local stringify_slk = require 'stringify_slk'
local ui = require 'ui-builder.init'
local txt = (require 'w3xparser').txt
local ini = (require 'w3xparser').ini
local slk = (require 'w3xparser').slk
local lni = require 'lni-c'

local ydwe = fs.ydwe_path():parent_path():remove_filename():remove_filename() / "Component"
if not fs.exists(ydwe) then
	ydwe = fs.ydwe_path()
end

local root = ydwe / 'share' / 'mpq'
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
	return true
end

local data, string
function loader:triggerdata(name, callback)
	log.trace("virtual_mpq 'triggerdata'")
	if #self.list == 0 then
		return nil
	end
	local t = nil
	for _, path in ipairs(self.list) do
		if fs.exists(path / 'ui') then
			t = ui.merge(t, ui.old_reader(path / 'ui'))
		else
			t = ui.merge(t, ui.new_reader(path))
		end
	end
	data, string =  ui.old_writer(t)
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
	local t = ini(io.load(root / 'units' / 'ui' / 'WorldEditStrings.txt'), 'WorldEditStrings')
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
			txt(io.load(root / 'units' / filename), filename, t)
		end
		txt(io.load(root / 'units' / 'ui' / 'ydwetip.txt'), 'ydwetip', t)
		
		local replace = {}
		txt(io.load(root / 'units' / 'ui' / 'editorsuffix.txt'), 'editorsuffix', replace)
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
		local t = slk(io.load(root / 'units' / 'units' / 'abilitydata.slk'), 'abilitydata.slk')
		for _, o in pairs(t) do
			o.useInEditor = 1
		end
		return stringify_slk(t, 'alias')
	end)
	virtual_mpq.watch('units\\abilitybuffdata.slk', function ()
		local t = slk(io.load(root / 'units' / 'units' / 'abilitybuffdata.slk'), 'abilitybuffdata.slk')
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
	virtual_mpq.event(function(_, name)
		log.info('OpenPathAsArchive', name)
	end)
end

uiloader = loader
