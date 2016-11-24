require "registry"
require "util"
local ui = require 'ui-builder.init'

local loader = {}

function loader:loadfile(path)
	local f = assert(io.open(path, "r"))
	local tbl = {}
	local section = nil
	for line in f:lines() do
		if string.sub(line,1,1) == "[" then
			section = string.sub(line, 2, string.len(line) - 1)
			tbl[section] = {}
		elseif string.sub(line,1,2) == "//" then
		elseif line ~= "" then
			local first = string.find(line, "=")
			if first then
				local key = string.sub(line, 1, first - 1)
				local value = string.sub(line, first + 1) or ""
				table.insert(tbl[section], { key, value })
			end
		end
	end
	f:close()
	return tbl
end

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
	
	local root = fs.ydwe_path():parent_path():remove_filename():remove_filename() / "Component" / "share" / "mpq"
	if not fs.exists(root) then
		root = fs.ydwe_path() / 'share' / 'mpq'
	end
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

local function table_append(a, b)
	for _, bv in ipairs(b) do
		for i, av in ipairs(a) do
			if av[1] == bv[1] then
				table.remove(a, i)
				break
			end
		end
	end
	local pos = 1
	for _, v in ipairs(b) do
		table.insert(a, pos, v)
		pos = pos + 1
	end
end

function loader:worldeditstrings()
	log.trace("virtual_mpq 'worldeditstrings'")
	local tbl = self:loadfile(fs.ydwe_path() / 'share' / 'mpq' / 'units' / 'ui' / 'WorldEditStrings.txt')
	table_append(tbl.WorldEditStrings, {
		{ 'WESTRING_APPNAME', 'YD WorldEdit [ ' .. tostring(ydwe_version) .. ' ]' }
	})
	local rt = {}
	table.insert(rt, "[WorldEditStrings]")
	for _, v in ipairs(tbl.WorldEditStrings) do
		table.insert(rt, v[1] .. "=" .. v[2])
	end
	return table.concat(rt, '\n')
end

function loader:initialize()
	self:config()
	virtual_mpq.watch('UI\\TriggerData.txt',      function (name) return self:triggerdata() end)
	virtual_mpq.watch('UI\\TriggerStrings.txt',   function (name) return self:triggerstrings() end)
	virtual_mpq.watch('UI\\WorldEditStrings.txt', function (name) return self:worldeditstrings() end)
end

uiloader = loader
