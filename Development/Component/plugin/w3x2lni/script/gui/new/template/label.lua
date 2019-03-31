local gui = require 'yue.gui'
local ca = require 'gui.new.common_attribute'

return function (t, data)
    local label = gui.Label.create('')
    if t.bind and t.bind.text then
        local bind_text
        bind_text = data:bind(t.bind.text, function()
            label:settext(bind_text:get())
        end)
        label:settext(bind_text:get())
    else
        label:settext(t.text or '')
    end
    if t.style then
        label:setstyle(t.style)
    end
    if t.align then
        label:setalign(t.align)
    end
    local bind = {}
    if t.bind and t.bind.text_color then
        bind.text_color = data:bind(t.bind.text_color, function()
            label:setcolor(bind.text_color:get())
        end)
        label:setcolor(bind.text_color:get())
    else
        if t.text_color then
            label:setcolor(t.text_color)
        end
    end
    ca.font(label, t)
    ca.label_color(label, t, data, bind)
    return label
end
