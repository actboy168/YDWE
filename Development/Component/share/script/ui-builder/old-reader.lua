local mt = {}
mt.__index = mt

local function switch(value)
    return function (mapping)
        if mapping[value] then
            mapping[value]()
        end
    end
end

local function str_trim(str, p)
    return str:gsub('^'..p..'*(.-)$', '%1'):gsub('^(.-)'..p..'*$', '%1')
end

local function str_split(str, p)
    local rt = {}
    local l = 1
    local state = 0
    for i = 1, #str do
        local c = str:sub(i,i)
        if state == 0 then
            if c == '"' then
                state = 1
            elseif c == p then
                table.insert(rt, str:sub(l, i - 1))
                l = i + 1
            end
        elseif state == 1 then
            if c == '"' then
                state = 0
            end
        end
    end
    if l <= #str then
        table.insert(rt, str:sub(l, #str))
    end
    return rt
end

function mt:accept_ui(type, name, key, value)
    if not self.ui[type][name] then
        self.ui[type][name] = { name = name }
    end
    local ui = self.ui[type][name]
    switch(key) {
        args = function ()
            local r = str_split(value, ',')
            if type == 'call' then
                ui.use_in_event = r[2]
                ui.returns = r[3]
                table.remove(r, 1)
                table.remove(r, 1)
                table.remove(r, 1)
            else
                table.remove(r, 1)
            end
            ui.args = {}
            for _, v in ipairs(r) do
                table.insert(ui.args, { type = v })
            end
        end,
        default = function ()
            local r = str_split(value, ',')
            for i, v in ipairs(ui.args) do
                if r[i] and r[i] ~= '_' then
                    v.default = r[i]
                end
            end
        end,
        limit = function ()
            local r = str_split(value, ',')
            for i, v in ipairs(ui.args) do
                if r[2*i-1] and r[2*i-1] ~= '_' then
                    v.min = r[2*i-1]
                end
                if r[2*i] and r[2*i] ~= '_' then
                    v.max = r[2*i]
                end
            end
        end,
        category = function ()
            ui.category = value
            if not self.categories[type][ui.category] then
                self.categories[type][ui.category] = {}
                table.insert(self.categories[type], ui.category)
            end
            table.insert(self.categories[type][ui.category], ui)
        end,
        script_name = function ()
            ui.script_name = value
        end,
        description = function ()
            if not ui.title then
                ui.title = str_trim(value, '"')
                return
            end
            if ui.description then
                return
            end
            local str = ''
            local r = str_split(value, ',')
            for _, s in ipairs(r) do
                s = str_trim(s, '%s')
                if s:sub(1,1) == '~' then
                    str = str .. '${' .. s:sub(2) .. '}'
                else
                    str = str .. str_trim(s, '"')
                end
            end
            ui.description = str
        end,
        comment = function ()
            ui.comment = str_trim(value, '"')
        end,
    }
end

function mt:parse_ui(type, subtype, key, value)
    if subtype == 'data' then
        if key:sub(1,1) ~= '_' then
            self:accept_ui(type, key, 'args', value)
        elseif key:sub(- #'_Defaults', -1) == '_Defaults' then
            self:accept_ui(type, key:sub(2, -1 - #'_Defaults'), 'default', value)
        elseif key:sub(- #'_Limits', -1) == '_Limits' then
            self:accept_ui(type, key:sub(2, -1 - #'_Limits'), 'limit', value)
        elseif key:sub(- #'_Category', -1) == '_Category' then
            self:accept_ui(type, key:sub(2, -1 - #'_Category'), 'category', value)
        elseif key:sub(- #'_ScriptName', -1) == '_ScriptName' then
            self:accept_ui(type, key:sub(2, -1 - #'_ScriptName'), 'script_name', value)
        elseif key:sub(- #'_UseWithAI', -1) == '_UseWithAI' then
        elseif key:sub(- #'_AIDefaults', -1) == '_AIDefaults' then
        else
            error('无法解析: ' .. key)
        end
    elseif subtype == 'string' then
        if key:sub(- #'Hint', -1) == 'Hint' then
            self:accept_ui(type, key:sub(1, -1 - #'Hint'), 'comment', value)
        else
            self:accept_ui(type, key, 'description', value)
        end
    else
        error('未知类型: ' .. subtype)
    end
end

function mt:parse_define(section, key, value)
    if not self.ui.define[section] then
        self.ui.define[section] = {}
    end
    table.insert(self.ui.define[section], {key, value})
end

function mt:parse(section, key, value)
    switch(section) {
        TriggerEvents = function ()
            self:parse_ui('event', 'data', key, value)
        end,
        TriggerEventStrings = function ()
            self:parse_ui('event', 'string', key, value)
        end,
        TriggerConditions = function ()
            self:parse_ui('condition', 'data', key, value)
        end,
        TriggerConditionStrings = function ()
            self:parse_ui('condition', 'string', key, value)
        end,
        TriggerActions = function ()
            self:parse_ui('action', 'data', key, value)
        end,
        TriggerActionStrings = function ()
            self:parse_ui('action', 'string', key, value)
        end,
        TriggerCalls = function ()
            self:parse_ui('call', 'data', key, value)
        end,
        TriggerCallStrings = function ()
            self:parse_ui('call', 'string', key, value)
        end,
        TriggerCategories = function ()
            self:parse_define(section, key, value)
        end,
        TriggerTypes = function ()
            self:parse_define(section, key, value)
        end,
        TriggerTypeDefaults = function ()
            self:parse_define(section, key, value)
        end,
        TriggerParams = function ()
            self:parse_define(section, key, value)
        end,
        DefaultTriggerCategories = function ()
            self:parse_define(section, key, value)
        end,
        DefaultTriggers = function ()
            self:parse_define(section, key, value)
        end,
        AIFunctionStrings = function ()
            self:parse_define(section, key, value)
        end,
    }
end

function mt:readfile(path)
	local f = assert(io.open(path, "r"))
	local section = nil
	for line in f:lines() do
		if line:sub(1,1) == "[" then
			section = line:sub(2, #line - 1)
		elseif line:sub(1,2) == "//" then
		elseif line ~= "" then
			local first = line:find("=")
			if first then
				local key = line:sub(1, first - 1)
				local value = line:sub(first + 1) or ""
				self:parse(section, key, value)
			end
		end
	end
	f:close()
end

function mt:reset()
    self.ui = {
        define = {},
        event = {},
        condition = {},
        action = {},
        call = {},
    }
    self.categories = {
        event = {},
        condition = {},
        action = {},
        call = {},
    }
end

function mt:read(path)
    self:reset()
    self:readfile(path / [[TriggerData.txt]])
    self:readfile(path / [[TriggerStrings.txt]])
end

return function(path)
    local obj = setmetatable({}, mt)
    obj:read(path)
    return obj
end
