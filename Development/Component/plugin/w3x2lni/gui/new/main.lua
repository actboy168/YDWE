local gui = require 'yue.gui'
local ext = require 'yue-ext'
local timer = require 'gui.timer'
local lang = require 'tool.lang'
local config = require 'tool.config' ()
local input_path = require 'tool.input_path'

lang:set_lang(config.global.lang)
window = {}

ext.on_timer = timer.update
function ext.on_dropfile(filename)
    if window._worker and not window._worker.exited then
        return
    end
    window:show_page('select')
    window._filename = input_path(filename)
end

local FontPool = {}
function Font(name, size, weight, style)
    weight = weight or 'normal'
    style = style or 'normal'
    local key = ('%s|%d|%s|%s'):format(name, size, weight, style)
    local r = FontPool[key]
    if not r then
        r = gui.Font.create(name, size, weight, style)
        FontPool[key] = r
    end
    return r
end


function Label(text, color)
    if not color then
        color = window._color
    end
    local label = gui.Label.create(text)
    label:setbackgroundcolor(color)
    label:setfont(Font('黑体', 16))
    return label
end

function Button(text, color1, color2)
    local btn = gui.Button.create(text)
    local function make_color()
        if not color1 then
            color1 = window._color
        end
        if not color2 then
            if #color1 == 4 then
                color2 = ('#%01X%01X%01X'):format(
                    math.min(tonumber(color1:sub(2, 2), 16) + 0x1, 0xF),
                    math.min(tonumber(color1:sub(3, 3), 16) + 0x1, 0xF),
                    math.min(tonumber(color1:sub(4, 4), 16) + 0x1, 0xF)
                )
            elseif #color1 == 7 then
                color2 = ('#%02X%02X%02X'):format(
                    math.min(tonumber(color1:sub(2, 3), 16) + 0x10, 0xFF),
                    math.min(tonumber(color1:sub(4, 5), 16) + 0x10, 0xFF),
                    math.min(tonumber(color1:sub(6, 7), 16) + 0x10, 0xFF)
                )
            end
        end
        btn:setbackgroundcolor(color1)
        btn._backgroundcolor1 = color1
        btn._backgroundcolor2 = color2
    end
    make_color()
    function btn:onmouseleave()
        self:setbackgroundcolor(self._backgroundcolor1)
    end
    function btn:onmouseenter()
        self:setbackgroundcolor(self._backgroundcolor2)
    end
    function btn:update_color(c1, c2)
        color1 = c1
        color2 = c2
        make_color()
    end
    return btn
end

function Progress(color)
    local view = gui.Container.create()
    view:setbackgroundcolor '#444'

    local bar = gui.Label.create('')
    bar:setstyle { FlexGrow = 0 }
    view:addchildview(bar)
    local function make_color()
        if not color then
            color = window._color
        end
        bar:setbackgroundcolor(color)
    end
    make_color()
    function view:update_color(c)
        color = c
        make_color()
    end
    function view:set_progress(n)
        bar:setstyle { FlexGrow = n / 100 }
    end
    return view
end

function window:close_theme()
    self._close._backgroundcolor1 = self._color
    self._close:setbackgroundcolor(self._close._backgroundcolor1)
end

function window:addcaption(w)
    local caption = gui.Container.create()
    caption:setmousedowncanmovewindow(true)
    caption:setstyle { Height = 40, FlexDirection = 'row', JustifyContent = 'space-between' }
    self._caption = caption
    g_caption = caption
    local title = gui.Label.create('W3x2Lni')
    title:setmousedowncanmovewindow(true)
    title:setstyle { Width = 120 }
    title:setfont(Font('Constantia', 24, 'bold'))
    caption:addchildview(title)
    self._title = title

    local close = gui.Container.create()
    close:setstyle { Margin = 0, Width = 40 }

    local canvas = gui.Canvas.createformainscreen{width=40, height=40}
    local painter = canvas:getpainter()
    painter:setstrokecolor('#000000')
    painter:beginpath()
    painter:moveto(15, 15)
    painter:lineto(25, 25)
    painter:moveto(15, 25)
    painter:lineto(25, 15)
    painter:closepath()
    painter:stroke()
    close._backgroundcolor1 = '#000000'
    close:setbackgroundcolor('#000000')
    function close:onmouseleave()
        self:setbackgroundcolor(self._backgroundcolor1)
    end
    function close:onmouseenter()
        self:setbackgroundcolor('#BE3246')
    end
    function close:onmousedown()
        w:close()
    end
    function close.ondraw(self, painter, dirty)
      painter:drawcanvas(canvas, {x=0, y=0, width=40, height=40})
    end

    self._close = close
    
    caption:addchildview(close)
    return caption
end

function window:create(t)
    local win = gui.Window.create { frame = false }
    function win.onclose()
        gui.MessageLoop.quit()
    end
    win:settitle('w3x2lni')
    ext.register_window('w3x2lni')

    local view = gui.Container.create()
    view:setbackgroundcolor('#222')
    view:setstyle { Padding = 1 }
    local caption = self:addcaption(win)
    view:addchildview(caption)
    win:sethasshadow(true)
    win:setresizable(false)
    win:setmaximizable(false)
    win:setminimizable(false)
    win:setcontentview(view)
    win:setcontentsize { width = t.width, height = t.height }
    win:center()
    win:activate()
    self._window = win
end

function window:set_theme(title, color)
    self._title:settext(title)
    self._color = color
    self._caption:setbackgroundcolor(color)
    self:close_theme()
end

function window:show_page(name)
    local view = self._window:getcontentview()
    if self._page then
        self._page:setvisible(false)
    end
    self._page = require('gui.new.page.' .. name)
    self._page:setvisible(true)
    view:addchildview(self._page)
    if self._page.on_show then
        self._page:on_show()
    end
end

local view = window:create {
    width = 400, 
    height = 600,
}

window:set_theme('W3x2Lni', '#00ADD9')
window:show_page('index')

gui.MessageLoop.run()
