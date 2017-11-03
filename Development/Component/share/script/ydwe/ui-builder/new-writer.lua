local mt = {}
mt.__index = mt

function mt:stringify_section(str, t, section)
    if not t[section] then
        return
    end
    table.insert(str, ('[%s]'):format(section))
    for _, l in ipairs(t[section]) do
        table.insert(str, ('%s=%s'):format(l[1], l[2]))
    end
    table.insert(str, '')
end

function mt:stringify_define(t)
    local str = {}
    self:stringify_section(str, t, 'TriggerCategories')
    self:stringify_section(str, t, 'TriggerTypes')
    self:stringify_section(str, t, 'TriggerTypeDefaults')
    self:stringify_section(str, t, 'TriggerParams')
    self:stringify_section(str, t, 'DefaultTriggerCategories')
    self:stringify_section(str, t, 'DefaultTriggers')
    self:stringify_section(str, t, 'AIFunctionStrings')
    return table.concat(str, '\n')
end

local function constr(str)
    if str:find('%s') then
        return ('%q'):format(str)
    end
    return str
end

function mt:stringify_ui(t)
    local str = {}
    for _, category in ipairs(t) do
        for _, u in ipairs(t[category]) do
            table.insert(str, ('[%s]'):format(constr(u.name)))
            table.insert(str, ('title = %q'):format(u.title))
            table.insert(str, ('description = %q'):format(u.description))
            if u.comment then
                table.insert(str, ('comment = %q'):format(u.comment))
            end
            if u.script_name then
                table.insert(str, ('script_name = %s'):format(u.script_name))
            end
            table.insert(str, ('category = %s'):format(u.category))
            if u.use_in_event and u.use_in_event ~= '1' then
                table.insert(str, ('use_in_event = %s'):format(u.use_in_event))
            end
            if u.returns then
                table.insert(str, ('returns = %s'):format(u.returns))
            end
            for _, arg in ipairs(u.args) do
                table.insert(str, '[[.args]]')
                table.insert(str, ('type = %s'):format(arg.type))
                if arg.default then
                    table.insert(str, ('default = %q'):format(arg.default))
                end
                if arg.min then
                    table.insert(str, ('min = %s'):format(arg.min))
                end
                if arg.max then
                    table.insert(str, ('max = %s'):format(arg.max))
                end
            end
            table.insert(str, '')
        end
    end
    return table.concat(str, '\n')
end


function mt:write(t)
    return self:stringify_define(t.ui.define)
    , self:stringify_ui(t.categories.event)
    , self:stringify_ui(t.categories.condition)
    , self:stringify_ui(t.categories.action)
    , self:stringify_ui(t.categories.call)
end

return function(t)
    local obj = setmetatable({}, mt)
    return obj:write(t)
end
