local gui = require 'yue.gui'
local ca = require 'gui.new.common_attribute'

return function (t, data)
    local btn = gui.Button.create('')
    local bind = {}
    if t.bind and t.bind.title then
        local bind_title
        bind_title = data:bind(t.bind.title, function()
            btn:settitle(bind_title:get())
        end)
        btn:settitle(bind_title:get())
    else
        btn:settitle(t.title or '')
    end
    if t.style then
        btn:setstyle(t.style)
    end
    if t.on and t.on.click then
        function btn:onmousedown()
            t.on.click(self, t)
        end
    end
    ca.font(btn, t)
    ca.button_color(btn, btn, t, data, bind)
    ca.visible(btn, t, data, bind)
    return btn
end
