local lni = require 'lni-c'

local mt = {}
mt.__index = mt

local function switch(value)
    return function (mapping)
        if mapping[value] then
            mapping[value]()
        end
    end
end

function mt:read_ui(type, path)
	local f = io.open(path, "r")
    if not f then
        return
    end
    local lastkey
    local last
    local function savelast()
        if not last then
            return
        end
        last.name = lastkey
        if type == 'call' then
            if not last.use_in_event then
                last.use_in_event = '1'
            end
        end
        if not self.categories[type][last.category] then
            self.categories[type][last.category] = {}
            table.insert(self.categories[type], last.category)
        end
        table.insert(self.categories[type][last.category], last)
    end
    local t = setmetatable({}, {
        __newindex = function(_, key, value)
            self.ui[type][key] = value
            savelast()
            lastkey = key
            last = value
        end,
    })
    local buf = f:read 'a'
    f:close()
    lni(buf, type, t)
    savelast()
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

function mt:parse_define(section, key, value)
    if not self.ui.define[section] then
        self.ui.define[section] = {}
    end
    table.insert(self.ui.define[section], {key, value})
end

function mt:parse(section, key, value)
    switch(section) {
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

function mt:read_define(path)
	local f = io.open(path, "r")
    if not f then
        return
    end
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

function mt:read(path)
    self:reset()
    self:read_define(path / [[define.txt]])
    self:read_ui('event', path / [[event.txt]])
    self:read_ui('condition', path / [[condition.txt]])
    self:read_ui('action', path / [[action.txt]])
    self:read_ui('call', path / [[call.txt]])
end

return function(path)
    local obj = setmetatable({}, mt)
    obj:read(path)
    return obj
end
