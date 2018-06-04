
local function is_enable_japi()
	local ok, result = pcall(function ()
		local tbl = sys.ini_load(fs.ydwe_path() / 'plugin' / 'warcraft3' / 'config.cfg')
		return tbl['Enable']['yd_jass_api.dll'] ~= '0'
	end)
	if not ok then return true end
	return result
end

local function get_ui()
    local ydwe = fs.ydwe_devpath()
	local list = {}
	local f, err = io.open(ydwe / 'ui' / 'config', 'r')
	if not f then
		log.error('Open ' .. (ydwe / 'ui' / 'config'):string() .. ' failed.')
		return list
	end
	local enable_japi = is_enable_japi()
    for line in f:lines() do
        line = line:gsub("^%s*(.-)%s*$", "%1")
		if not enable_japi and line == 'japi' then
			-- do nothing
		else
			table.insert(list, ydwe / 'ui' / line)
		end
	end
	f:close()
	return list
end

local list = get_ui()
return list
