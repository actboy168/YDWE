local gui = require 'yue.gui'

return function (t)
    local o = gui.Container.create()
    if t.style then
        o:setstyle(t.style)
    end
    return o
end
