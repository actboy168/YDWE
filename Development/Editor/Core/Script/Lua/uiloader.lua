require "registry"
require "util"

local loader = {}

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
				if tbl[section][key] then
					table.insert(tbl[section][key], value)
				else
					tbl[section][key] = { value }
				end
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
			for key, value in pairs(keyvalue) do
				tbl[section][key] = value
			end
		end
	end
	for section, keyvalue in pairs(rem) do
		if tbl[section] then
			for key, value in pairs(keyvalue) do
				tbl[section][key] = nil
			end
		end
	end
	return tbl
end

function loader:save_section(tbl, rt, name)
	if tbl[name] then
		table.insert(rt, "[" .. name .. "]")
		for key, values in pairs(tbl[name]) do
			for _, value in ipairs(values) do
				table.insert(rt, key .. "=" .. value)
			end
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
	loader:save_section(tbl, rt, 'WorldEditStrings')
	return table.concat(rt, '\n')
end

function loader:config()
	self.list = {}
	local root = fs.ydwe_path() / 'share' / 'mpq'
	local f, err = io.open((root / 'config'):string())
	if not f then
		log.error('Open ' .. (root / 'config'):string() .. ' failed.')
		return false
	end
	for line in f:lines() do
		table.insert(self.list, root / string.trim(line))
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
		local from = self:loadfile((path / name):string())
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
