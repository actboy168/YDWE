local gui = require 'yue.gui'
local ext = require 'yue-ext'
local timer = require 'gui.timer'
local ev = require 'gui.event'
local ui = require 'gui.new.template'

window = {}

ext.on_timer = timer.update
function ext.on_dropfile(filename)
    if window._worker and not window._worker.exited then
        return
    end
    local input_path = require 'share.input_path'
    local builder = require 'map-builder'
    local war3 = require 'share.war3'
    if war3:open(fs.path(filename)) then
        window._filename = fs.path(filename)
        window._mode = 'mpq'
        window:set_theme('War3Dump', '#9CD')
        window:show_page('convert')
        window:set_theme('War3Dump', '#9CD')
        return
    end
    local mappath = input_path(filename)
    local map = builder.load(mappath)
    if not map then
        return
    end
    map:close()
    window._filename = mappath
    window:show_page('select')
end

local function create_mainview(win)
    local template = ui.container {
        color = '#222',
        style = { Padding = 1 },
        ui.container {
            id = 'caption',
            style = { Height = 40, FlexDirection = 'row', JustifyContent = 'space-between' },
            bind = {
                color = 'theme'
            },
            ui.label {
                id = 'title',
                style = { Width = 200 },
                align = 'start',
                font = { name = 'Constantia', size = 24, weight = 'bold' },
                bind = {
                    text = 'title'
                }
            },
            ui.container {
                id = 'close',
                style = { Margin = 0, Width = 40 },
                color_hover = '#BE3246',
                bind = {
                    color = 'theme'
                }
            }
        }
    }
    
    local view, data, element = ui.create(template, {
        title = 'W3x2Lni',
        theme = '#00ADD9',
    })
    
    ev.on('update theme', function(color, title)
        data.theme = color
        data.title = title
    end)
    
    element.caption:setmousedowncanmovewindow(true)
    element.title:setmousedowncanmovewindow(true)

    function element.close:onmousedown()
        win:close()
    end
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
    function element.close:ondraw(painter, dirty)
        painter:drawcanvas(canvas, {x=0, y=0, width=40, height=40})
    end
    return view
end

function window:create(t)
    local win = gui.Window.create { frame = false }
    function win.onclose()
        gui.MessageLoop.quit()
    end
    win:settitle('w3x2lni')
    ext.register_window('w3x2lni')
    win:sethasshadow(true)
    win:setresizable(false)
    win:setmaximizable(false)
    win:setminimizable(false)
    win:setcontentview(create_mainview(win))
    win:setcontentsize { width = t.width, height = t.height }
    win:center()
    win:activate()
    self._window = win
end

function window:set_theme(title, color)
    self._color = color
    ev.emit('update theme', color, title)
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
