local gui = require 'yue.gui'

return function (t, data)
    local scroll = gui.Scroll.create()
    if t.style then
        scroll:setstyle(t.style)
    end
    t.hpolicy = t.hpolicy or 'always'
    t.vpolicy = t.vpolicy or 'always'
    scroll:setscrollbarpolicy(t.hpolicy, t.vpolicy)
    local width = 0
    local height = 0
    local function update_size()
        scroll:setcontentsize { width = width, height = height } 
    end
    local bind = {}
    if t.bind and t.bind.width then
        bind.width = data:bind(t.bind.width, function()
            width = bind.width:get()
            update_size()
        end)
        width = bind.width:get()
    else
        width = t.width or 0
    end
    if t.bind and t.bind.height then
        bind.height = data:bind(t.bind.height, function()
            height = bind.height:get()
            update_size()
        end)
        height = bind.height:get()
    else
        height = t.height or 0
    end
    update_size()
    return scroll, scroll.setcontentview
end
