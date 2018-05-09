local databinding = require 'gui.new.databinding'

local function create_template(t, data)
    local name = t[1]
    local create_control = require ('gui.new.template.' .. name)
    local view, addchild = create_control(t, data)
    for i = 2, #t do
        t[i].font = t[i].font or t.font
        local child = create_template(t[i], data)
        if addchild then
            addchild(view, child)
        else
            view:addchildview(child)
        end
    end
    return view
end

return function (t)
    local data = databinding(t.data)
    return create_template(t, data), data.proxy
end
