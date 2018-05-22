local databinding = require 'gui.new.databinding'

local function create_template(t, data, element)
    local create_control = require ('gui.new.template.' ..  t.class)
    local view, addchild = create_control(t, data)
    for i = 1, #t do
        t[i].font = t[i].font or t.font
        local child = create_template(t[i], data, element)
        if addchild then
            addchild(view, child)
        else
            view:addchildview(child)
        end
    end
    if t.id then
        element[t.id] = view
    end
    return view
end

local ui = {}

function ui:__index(name)
    local function reg(t)
        t.class = name
        return t
    end
    rawset(self, name, reg)
    return reg
end

local function create(t, dataTemplate)
    local element = {}
    local data = databinding(dataTemplate)
    return create_template(t, data, element), data.proxy, element
end

local function createEx(t, data)
    local element = {}
    return create_template(t, data, element), data.proxy, element
end

return setmetatable({
    create = create,
    createEx = createEx,
}, ui)
