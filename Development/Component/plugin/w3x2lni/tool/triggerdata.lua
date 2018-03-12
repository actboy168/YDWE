local ui = require 'ui-builder'

local function string_trim (self) 
	return self:gsub("^%s*(.-)%s*$", "%1")
end

local function ydwe_path()
    require 'registry'
    local commands = registry.current_user() / [[SOFTWARE\Classes\YDWEMap\shell\run_war3\command]]
    if not commands then
        return nil
    end
    local command = commands['']
    local path = command:match '^"([^"]*)"'
    local ydpath = fs.path(path):remove_filename()
    if fs.exists(ydpath / 'YDWE.exe') then
        return ydpath
    else
        local ydpath = ydpath:remove_filename()
        if fs.exists(ydpath / 'YDWE.exe') then
            return ydpath
        end
    end
    return nil
end

local function trigger_config(mpq_path)
	local list = {}
	local f, err = io.open((mpq_path / 'config'):string(), 'r')
	if not f then
		return nil
    end
	for line in f:lines() do
		table.insert(list, mpq_path / string_trim(line))
	end
    f:close()
    return list
end

local function load_triggerdata(list)
    if not list or #list == 0 then
        return nil
    end
	local t = nil
	for _, path in ipairs(list) do
		if fs.exists(path / 'ui') then
			t = ui.merge(t, ui.old_reader(function(filename)
				return io.load(path / 'ui' / filename)
			end))
		else
			t = ui.merge(t, ui.new_reader(function(filename)
				return io.load(path / filename)
			end))
		end
	end
	return t
end

return function ()
    local path = ydwe_path()
    if not path then
        return nil, '请设置YDWE关联地图'
    end
    local list = trigger_config(path / 'share' / 'ui') or trigger_config(path / 'share' / 'mpq')
    if not list then
        return nil, '没有找到触发器数据的目录：' .. path:string()
    end
    local state = load_triggerdata(list)
    if not state then
        return nil, '没有读取到触发器数据'
    end
    return state
end