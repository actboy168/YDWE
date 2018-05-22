local gui = require 'yue.gui'
local ev = require 'gui.event'
local ca = require 'gui.new.common_attribute'

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
    label:setalign 'start'
    ca.font(label, t)
    return label
end

local function tree_button(t, children)
    local btn = gui.Container.create()
    btn:setstyle { Height = 24, FlexGrow = 1 }
    btn.select = t.select or false
    local function update_select()
        if btn.select then
            children:setvisible(true)
        else
            children:setvisible(false)
        end
    end
    update_select()
    function btn:onmousedown()
        self.select = not self.select
        update_select()
        self:schedulepaint()
    end
    btn:addchildview(tree_label(t))
    tree_icon(btn)
    return btn
end

local function tree_children(t, btn)
    return gui.Container.create()
end

return function (t, data)
    local o = gui.Container.create()
    if t.style then
        o:setstyle(t.style)
    end
    local children = tree_children(t)
    children:setstyle { Padding = 4 }
    local btn = tree_button(t, children)
    local bind = {}
    ca.button_color(btn, btn, t, data, bind)
    o:addchildview(btn)
    o:addchildview(children)
    return o, function (self, child)
        children:addchildview(child)
    end
end
