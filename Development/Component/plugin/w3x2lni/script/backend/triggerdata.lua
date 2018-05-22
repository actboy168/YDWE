require 'filesystem'
local ui = require 'ui-builder'
local lang = require 'share.lang'

local root = fs.current_path()

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
    if not command then
        return nil
    end
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
    if not fs.exists(mpq_path) then
        return nil
    end
	local list = {}
	local f, err = io.open((mpq_path / 'config'):string(), 'r')
	if not f then
		return { mpq_path, type = 'old' }
    end
	for line in f:lines() do
		table.insert(list, mpq_path / string_trim(line))
	end
    f:close()
    list.type = 'new'
    return list
end

local function load_triggerdata(list)
    if not list or #list == 0 then
        return nil
    end
    local t = nil
    local ok
    for _, path in ipairs(list) do
        if list.type == 'old' then
            t = ui.merge(t, ui.old_reader(function(filename)
                local buf = io.load(path / 'ui' / filename) or io.load(path / filename)
                if buf then
                    ok = true
                end
                return buf
            end))
        else
            t = ui.merge(t, ui.new_reader(function(filename)
                local buf = io.load(path / filename)
                if buf then
                    ok = true
                end
                return buf
            end))
        end
    end
    if not ok then
        return nil
    end
    return t
end

local function load_ydew()
    local path = ydwe_path()
    if not path then
        return nil, lang.script.NEED_YDWE_ASSOCIATE
    end
    local list = trigger_config(path / 'ui') or trigger_config(path / 'share' / 'ui') or trigger_config(path / 'share' / 'mpq')
    if not list then
        return nil, lang.script.NO_TRIGGER_DATA_DIR .. path:string()
    end
    local suc, state = pcall(load_triggerdata, list)
    if not suc then
        return nil, lang.script.TRIGGER_DATA_ERROR
    end
    if not state then
        return nil, lang.script.NO_TRIGGER_DATA
    end
    return state
end

local function load_ui(ui)
    local path = root:parent_path() / 'data' / ui / 'we' / 'ui'
    local list = trigger_config(path)
    if not list then
        return nil, lang.script.NO_TRIGGER_DATA_DIR .. path:string()
    end
    local suc, state = pcall(load_triggerdata, list)
    if not suc then
        return nil, lang.script.TRIGGER_DATA_ERROR
    end
    if not state then
        return nil, lang.script.NO_TRIGGER_DATA
    end
    return state
end

return function (ui)
    if ui == '${YDWE}' then
        return load_ydew()
    else
        return load_ui(ui)
    end
end
