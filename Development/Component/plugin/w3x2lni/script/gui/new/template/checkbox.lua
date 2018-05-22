local gui = require 'yue.gui'
local ev = require 'gui.event'
local ca = require 'gui.new.common_attribute'

local function checkbox_button(t, data)
    local btn = gui.Button.create('')
    local lbl = gui.Label.create('')
    btn:setstyle { Width = 20, Height = 20 }
    btn:setbackgroundcolor('#333743')
    lbl:setstyle { Width = 16, Height = 16, Position = 'absolute', Left = 2, Top = 2 }

    local function update_color()
        if btn.value then
            lbl:setvisible(true)
        else
            lbl:setvisible(false)
        end
    end
    
    local bind = {}
    if t.bind and t.bind.value then
        bind.value = data:bind(t.bind.value, function()
            btn.value = bind.value:get()
            update_color()
        end)
        btn.value = bind.value:get()
        update_color()
    else
        if type(t.value) ~= 'nil' then
            btn.value = t.value
            update_color()
        else
            btn.value = false
            update_color()
        end
    end

    ca.button_color(btn, lbl, t, data, bind)
    return btn, lbl, function()
        btn.value = not btn.value
        update_color()
        if bind.value then
            bind.value:set(btn.value)
        end
    end
end

local function checkbox_label(t)
    local label = gui.Label.create(t.text)
    label:setcolor('#AAA')
    label:setalign 'start'
    label:setstyle { FlexGrow = 1 }
    ca.font(label, t)
    return label
end

return function (t, data)
    local o = gui.Container.create()
    o:setstyle { FlexDirection = 'row' }
    if t.style then
        o:setstyle(t.style)
    end
    local btn, lbl, onclick = checkbox_button(t, data)
    o:addchildview(btn)
    o:addchildview(lbl)
    o:addchildview(checkbox_label(t))
    ca.event(o, t, data, 'mouseenter')
    ca.event(o, t, data, 'mouseleave')
    function btn:onmousedown()
        onclick()
    end
    function o:onmousedown()
        onclick()
    end
    return o
end
