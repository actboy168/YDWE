require "registry"
require "util"

local loader = {}

local function table_remove(a, k)
	for i, v in ipairs(a) do
		if v[1] == k then
			table.remove(a, i)
		end
	end
end

local function table_append(a, b)
	for _, v in ipairs(b) do
		table_remove(a, v[1])
	end
	local pos = 1
	for _, v in ipairs(b) do
		table.insert(a, pos, v)
		pos = pos + 1
	end
end

function loader:loadfile(path)
	local f, e = io.open(path, "r")
	if not f then
		return nil, e
	end
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

function loader:merge(tbl, from)
	local rem = {}
	for section, keyvalue in pairs(from) do
		if string.sub(section, 1, 7) == 'remove:' then
			rem[string.sub(section, 8, -1)] = keyvalue
		else
			if not tbl[section] then
				tbl[section] = {}
			end
			table_append(tbl[section], keyvalue)
		end
	end
	for section, keyvalue in pairs(rem) do
		if tbl[section] then
			for key, _ in pairs(keyvalue) do
				table_remove(tbl[section], key)
			end
		end
	end
	return tbl
end

function loader:save_section(tbl, rt, name)
	if tbl[name] then
		table.insert(rt, "[" .. name .. "]")
		for _, v in ipairs(tbl[name]) do
			table.insert(rt, v[1] .. "=" .. v[2])
		end
	end
end

function loader:save_triggerdata(tbl)
	local rt = {}
	loader:save_section(tbl, rt, 'TriggerCategories')
	loader:save_section(tbl, rt, 'TriggerTypes')
	loader:save_section(tbl, rt, 'TriggerTypeDefaults')
	loader:save_section(tbl, rt, 'TriggerParams')
	loader:save_section(tbl, rt, 'TriggerEvents')
	loader:save_section(tbl, rt, 'TriggerConditions')
	loader:save_section(tbl, rt, 'TriggerActions')
	loader:save_section(tbl, rt, 'TriggerCalls')
	loader:save_section(tbl, rt, 'DefaultTriggerCategories')
	loader:save_section(tbl, rt, 'DefaultTriggers')
	return table.concat(rt, '\n')
end

function loader:save_triggerstrings(tbl)
	local rt = {}
	loader:save_section(tbl, rt, 'TriggerEventStrings')
	loader:save_section(tbl, rt, 'TriggerConditionStrings')
	loader:save_section(tbl, rt, 'TriggerActionStrings')
	loader:save_section(tbl, rt, 'TriggerCallStrings')
	loader:save_section(tbl, rt, 'AIFunctionStrings')
	return table.concat(rt, '\n')
end

function loader:save_worldeditstrings(tbl)
	local rt = {}
	tbl = self:merge(tbl, {
		WorldEditStrings = {
			{ 'WESTRING_APPNAME', 'YD WorldEdit [ ' .. tostring(ydwe_version) .. ' ]' }
		}
	})
	loader:save_section(tbl, rt, 'WorldEditStrings')
	return table.concat(rt, '\n')
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
	local root = fs.ydwe_path() / 'share' / 'mpq'
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

function loader:watch(name, callback)
	log.trace("virtual_mpq '" .. name .. "'")
	if #self.list == 0 then
		return nil
	end
	local tbl = {}
	for _, path in ipairs(self.list) do
		local from = self:loadfile(path / name)
		if from then
			tbl = self:merge(tbl, from)
		end
	end
	return callback(self, tbl)
end

function loader:initialize()
	self:config()
	virtual_mpq.watch('UI\\TriggerData.txt',      function (str) return self:watch(str, self.save_triggerdata) end)
	virtual_mpq.watch('UI\\TriggerStrings.txt',   function (str) return self:watch(str, self.save_triggerstrings) end)
	virtual_mpq.watch('UI\\WorldEditStrings.txt', function (str) return self:watch(str, self.save_worldeditstrings) end)
end

uiloader = loader
