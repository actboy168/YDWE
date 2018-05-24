local gui = require 'yue.gui'
local ca = require 'gui.new.common_attribute'
local timer = require 'gui.timer'

return function (t, data)
    local view = gui.Container.create()
    if t.style then
        view:setstyle(t.style)
    end
    view:setbackgroundcolor '#444'
    local frontlabel = gui.Label.create('')
    view:addchildview(frontlabel)
    local frontground = 0
    local background = 0
    local ti
    local function set_progress(n, force)
        if force then
            frontground = n
            background = n
            if ti then ti:remove() end
            ti = nil
            frontlabel:setstyle { FlexGrow = n }
            return
        end
        if n >= 1 then
            set_progress(1, true)
            return
        end
        if n < background or n <= 0 then
            set_progress(n, true)
            return
        end
        background = n
        if not ti then
            ti = timer.loop(30, function ()
                local delta1 = 0
                local delta2 = 0
                if frontground < background then
                    delta1 = (background - frontground) / 10
                    delta2 = math.min(background - frontground, 0.001)
                end
                frontground = frontground + math.max(delta1, delta2)
                frontlabel:setstyle { FlexGrow = frontground }
            end)
        end
    end
    local bind = {}
    if t.bind and t.bind.value then
        bind.value = data:bind(t.bind.value, function()
            set_progress(bind.value:get())
        end)
        set_progress(bind.value:get(), true)
    else
        set_progress(t.value or 0, true)
    end
    ca.visible(view, t, data, bind)
    ca.label_color(frontlabel, t, data, bind)
    return view
end
