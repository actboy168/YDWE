local gui = require 'yue.gui'
local lang = require 'tool.lang'

local main = gui.Container.create()
main:setstyle { FlexGrow = 1 }

local view = gui.Container.create()
view:setstyle { FlexGrow = 1 }
main:addchildview(view)

local label = Label(lang.ui.AUTHOR)
label:setstyle { MarginTop = 20, Height = 28, Width = 240 }
view:addchildview(label)

local label = Label(lang.ui.FRONTEND .. 'actboy168', '#222')
label:setcolor('#AAA')
label:setstyle { MarginTop = 5, Height = 28, Width = 240 }
view:addchildview(label)

local label = Label(lang.ui.BACKEND .. lang.ui.SUMNEKO, '#222')
label:setcolor('#AAA')
label:setstyle { Height = 28, Width = 240 }
view:addchildview(label)

local label = Label(lang.ui.CHANGE_LOG)
label:setstyle { Height = 28, Width = 240 }
view:addchildview(label)


local changelog = gui.Scroll.create()
changelog:setstyle { FlexGrow = 1 }
changelog:setscrollbarpolicy('never', 'never')
view:addchildview(changelog)

local color  = {
    NEW = gui.Color.rgb(0, 173, 60),
    CHG = gui.Color.rgb(217, 163, 60),
    FIX = gui.Color.rgb(200, 30, 30),
    UI =  gui.Color.rgb(111, 77, 150),
}

local height = 0
local log = gui.Container.create()
for _, v in ipairs(require 'gui.changelog') do
    local label = gui.Label.create(v.version)
    label:setstyle { Margin = 3, Height = 25 }
    label:setbackgroundcolor('#444')
    label:setcolor('#AAA')
    label:setfont(Font('黑体', 18))
    log:addchildview(label)

    height = height + 31

    for _, l in ipairs(v) do
        local line = gui.Container.create()
        line:setstyle { Height = 31, FlexDirection = 'row' }

        local label = gui.Label.create(l[1])
        label:setbackgroundcolor(color[l[1]])
        label:setstyle { Margin = 3, Width = 40 }
        label:setfont(Font('黑体', 18))
        line:addchildview(label)

        local text = gui.Label.create(l[2])
        text:setcolor('#AAA')
        text:setstyle { Margin = 3, Width = 360, FlexGlow = 1 }
        text:setalign('start')
        text:setfont(Font('黑体', 18))
        line:addchildview(text)

        function text:onmouseleave()
            line:setbackgroundcolor('#222')
        end
        function text:onmouseenter()
            line:setbackgroundcolor('#444')
        end

        log:addchildview(line)

        height = height + 31
    end
end
changelog:setcontentview(log)
changelog:setcontentsize { width = 0, height = height }

local btn = Button(lang.ui.BACK)
btn:setstyle { Bottom = 0, Height = 28, Margin = 5 }
btn:setfont(Font('黑体', 16))
function btn:onclick()
    window:show_page('index')
end
main:addchildview(btn)

return main
