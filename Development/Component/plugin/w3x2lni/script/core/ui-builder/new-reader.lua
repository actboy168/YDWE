local lni = require 'lni'

local mt = {}
mt.__index = mt

local function switch(value)
    return function (mapping)
        if mapping[value] then
            mapping[value]()
        end
    end
end

local function get_lang_ui(loader, type, lang)
    if not lang then
        return
    end
    local filename = lang .. '\\' .. type .. '.txt'
    local buf = loader(filename)
    if buf then
        return lni(buf, filename)
    end
end

local function merge_lang_ui(key, value, lang_ui)
    local nv = lang_ui[key]
    if not nv then
        return value
    end
    for k, v in pairs(nv) do
        value[k] = v
    end
    return value
end

function mt:read_ui(loader, lang, type)
    local buf = loader(type .. '.txt')
    if not buf then
        return
    end
    local lang_ui = get_lang_ui(loader, type, lang)
    local lastkey
    local last
    local function savelast()
        if not last then
            return
        end
        if lang_ui then
            last = merge_lang_ui(lastkey, last, lang_ui)
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
    lni(buf, type .. '.txt', {t})
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

function mt:read_define(buf)
    if not buf then
        return
    end
	local section = nil
	for line in buf:gmatch '[^\r\n]+' do
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
end

function mt:read(loader, lang)
    self:reset()
    self:read_define(loader('define.txt'))
    if lang then
        self:read_define(loader(lang .. '\\define.txt'))
    end
    self:read_ui(loader, lang, 'event')
    self:read_ui(loader, lang, 'condition')
    self:read_ui(loader, lang, 'action')
    self:read_ui(loader, lang, 'call')
end

return function(loader, lang)
    local obj = setmetatable({}, mt)
    obj:read(loader, lang)
    return obj
end
