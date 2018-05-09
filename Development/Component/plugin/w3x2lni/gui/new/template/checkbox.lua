local gui = require 'yue.gui'
local ev = require 'gui.event'

local function getActiveColor(color)
    if #color == 4 then
        return ('#%01X%01X%01X'):format(
            math.min(tonumber(color:sub(2, 2), 16) + 0x1, 0xF),
            math.min(tonumber(color:sub(3, 3), 16) + 0x1, 0xF),
            math.min(tonumber(color:sub(4, 4), 16) + 0x1, 0xF)
        )
    elseif #color == 7 then
        return ('#%02X%02X%02X'):format(
            math.min(tonumber(color:sub(2, 3), 16) + 0x10, 0xFF),
            math.min(tonumber(color:sub(4, 5), 16) + 0x10, 0xFF),
            math.min(tonumber(color:sub(6, 7), 16) + 0x10, 0xFF)
        )
    else
        return color
    end
end

local function checkbox_button(t, data)
    local btn = gui.Button.create('')
    local lbl = gui.Label.create('')
    btn:setstyle { Width = 20, Height = 20 }
    lbl:setstyle { Width = 16, Height = 16, Position = 'absolute', Left = 2, Top = 2 }
    btn.hover = t.hover or false
    btn:setbackgroundcolor('#333743')
    lbl._backgroundcolor1 = window._color
    local function update_color()
        lbl._backgroundcolor2 = getActiveColor(lbl._backgroundcolor1)
        if btn.value then
            lbl:setvisible(true)
        else
            lbl:setvisible(false)
        end
        if btn.hover then
            lbl:setbackgroundcolor(lbl._backgroundcolor1)
        else
            lbl:setbackgroundcolor(lbl._backgroundcolor2)
        end
    end
    
    local d_select
    if type(t.value) == 'string' then
        d_select = data:bind(t.value, function()
            btn.value = d_select:get()
            update_color()
        end)
        btn.value = d_select:get()
    elseif type(t.value) == 'boolean' then
        btn.value = t.value
    else
        btn.value = false
    end
    update_color()

    function btn:onmousedown()
        btn.value = not btn.value
        update_color()
        if d_select then
            d_select:set(btn.value)
        end
    end
    function btn:onmouseleave()
        btn.hover = false
        lbl:setbackgroundcolor(lbl._backgroundcolor1)
    end
    function btn:onmouseenter()
        btn.hover = true
        lbl:setbackgroundcolor(lbl._backgroundcolor2)
    end
    ev.on('update theme', function()
        lbl._backgroundcolor1 = window._color
        update_color()
    end)
    return btn, lbl
end

local function checkbox_label(t)
    local label = gui.Label.create(t.text)
    label:setcolor('#AAA')
    if t.font then
        label:setfont(Font(t.font.name, t.font.size, t.font.weight, t.font.style))
    end
    label:setalign 'start'
    label:setstyle { FlexGrow = 1, MarginTop = 3 }
    return label
end

return function (t, data)
    local o = gui.Container.create()
    o:setstyle { FlexDirection = 'row' }
    if t.style then
        o:setstyle(t.style)
    end
    local btn, lbl = checkbox_button(t, data)
    o:addchildview(btn)
    o:addchildview(lbl)
    o:addchildview(checkbox_label(t))
    return o
end
