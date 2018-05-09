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

local function tree_icon(view)
    local canvas1 = gui.Canvas.createformainscreen{width=24, height=24}
    local painter = canvas1:getpainter()
    painter:setfillcolor('#CCC')
    painter:beginpath()
    painter:moveto(4, 4)
    painter:lineto(20, 4)
    painter:lineto(12, 20)
    painter:lineto(4, 4)
    painter:closepath()
    painter:fill()
    local canvas2 = gui.Canvas.createformainscreen{width=24, height=24}
    local painter = canvas2:getpainter()
    painter:setfillcolor('#CCC')
    painter:beginpath()
    painter:moveto(4, 4)
    painter:lineto(4, 20)
    painter:lineto(20, 12)
    painter:lineto(4, 4)
    painter:closepath()
    painter:fill()
    function view.ondraw(self, painter, dirty)
        if self.select then
            painter:drawcanvas(canvas1, {x=0, y=0, width=24, height=24})
        else
            painter:drawcanvas(canvas2, {x=0, y=0, width=24, height=24})
        end
    end
end

local function tree_label(t)
    local label = gui.Label.create(t.text)
    label:setstyle { Height = 24, Top = 2, Left = 24 }
    if t.font then
        label:setfont(Font(t.font.name, t.font.size, t.font.weight, t.font.style))
    end
    label:setalign 'start'
    return label
end

local function tree_button(t, children)
    local btn = gui.Container.create()
    btn:setstyle { Height = 24, FlexGrow = 1 }
    btn.select = t.select or false
    btn.hover = t.hover or false
    btn._backgroundcolor1 = window._color
    local function update_color()
        btn._backgroundcolor2 = getActiveColor(btn._backgroundcolor1)
        if btn.hover then
            btn:setbackgroundcolor(btn._backgroundcolor1)
        else
            btn:setbackgroundcolor(btn._backgroundcolor2)
        end
    end
    local function update_select()
        if btn.select then
            children:setvisible(true)
        else
            children:setvisible(false)
        end
    end
    update_color()
    update_select()
    function btn:onmousedown()
        self.select = not self.select
        update_select()
        self:schedulepaint()
    end
    function btn:onmouseleave()
        self.hover = false
        btn:setbackgroundcolor(btn._backgroundcolor1)
    end
    function btn:onmouseenter()
        self.hover = true
        btn:setbackgroundcolor(btn._backgroundcolor2)
    end
    ev.on('update theme', function()
        btn._backgroundcolor1 = window._color
        update_color()
    end)
    btn:addchildview(tree_label(t))
    tree_icon(btn)
    return btn
end

local function tree_children(t, btn)
    return gui.Container.create()
end

return function (t)
    local o = gui.Container.create()
    if t.style then
        o:setstyle(t.style)
    end
    local children = tree_children(t)
    local btn = tree_button(t, children)
    o:addchildview(btn)
    o:addchildview(children)
    return o, function (self, child)
        children:addchildview(child)
    end
end
