local gui = require 'yue.gui'
local ca = require 'gui.new.common_attribute'

return function (t, data)
    local o = gui.Container.create()
    if t.style then
        o:setstyle(t.style)
    end
    local bind = {}
    ca.label_color(o, t, data, bind)
    return o
end
