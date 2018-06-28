local gui = require 'yue.gui'

local fontDefault = gui.app:getdefaultfont()
local fontName = fontDefault:getname()
local fontSize = fontDefault:getsize()

local function guiThanksMain(contentCallback)
    local win = gui.Window.create { frame = false }
    local view = gui.Container.create()

    local caption = gui.Container.create()
    caption:setstyle { Height = 40, FlexDirection = 'row', JustifyContent = 'space-between' }
    caption:setbackgroundcolor('#28c')
    caption:setmousedowncanmovewindow(true)
    view:addchildview(caption)

    local title = gui.Label.create('感谢')
    caption:addchildview(title)
    title:setstyle { Height = 40, Left = 10, Width = 100 }
    title:setcolor('#eee')
    title:setalign 'start'
    title:setfont(gui.Font.create(fontName, 16, "bold", "normal"))
    title:setmousedowncanmovewindow(true)

    local close = gui.Container.create()
    caption:addchildview(close)
    close:setstyle { Width = 40 }
    function close:onmouseleave()
        self:setbackgroundcolor('#28c')
    end
    function close:onmouseenter()
        self:setbackgroundcolor('#BE3246')
    end
    function close:onmousedown()
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
    function close:ondraw(painter, dirty)
        painter:drawcanvas(canvas, {x=0, y=0, width=40, height=40})
    end

    local content = gui.Container.create()
    view:addchildview(content)
    contentCallback(content)

    win:sethasshadow(true)
    win:setresizable(false)
    win:setmaximizable(false)
    win:setminimizable(false)
    win:setalwaysontop(true)
    win:setcontentsize { width = 400, height = 700 }
    win:setcontentview(view)
    win:center()
    win:activate()
    function win:onclose()
        gui.MessageLoop.quit()
    end
    gui.MessageLoop.run()
end

local logo11 = {}
local logo11path = fs.ydwe_devpath() / 'bin' / '11logo.png'
if fs.exists(logo11path) then
    logo11.img = gui.Image.createfrompath(logo11path:string())
    local size = logo11.img:getsize()
    logo11.height = math.sqrt(10000.0 / size.width * size.height)
    logo11.width = logo11.height / size.height * size.width
end

local image = {}
local imagePool = {}

function image:create(name, url, path)
    if not imagePool[name] then
        local obj = {}
        if fs.exists(path) then
            obj.img = gui.Image.createfrompath(path:string())
            local size = obj.img:getsize()
            obj.height = math.sqrt(10000.0 / size.width * size.height)
            obj.width = obj.height / size.height * size.width
        end
        imagePool[name] = obj
    end
    local img = imagePool[name]

    local control
    if img.img then
        local height = img.height
        local width = img.width
        local canvas = gui.Canvas.createformainscreen{width=width, height=height}
        local painter = canvas:getpainter()
        painter:drawimage(img.img, {x=0, y=0, width=width, height=height})
    
        control = gui.Container.create()
        control:setstyle { Left = (400.0 - width) / 2.0, Width = width, Height = height }
        function control:ondraw(painter, dirty)
            painter:drawcanvas(canvas, {x=0, y=0, width=width, height=height})
        end
    else
        control = gui.Label.create(name)
        control:setcolor '#28c'
        control:setfont(gui.Font.create(fontName, 20, "normal", "normal"))
    end

    function control:onmousedown()
        os.execute('explorer "' .. url .. '"')
    end
    return control
end

return function ()
    guiThanksMain(function(content)
        content:setstyle { Margin = 10, FlexDirection = 'column', JustifyContent = 'center' }
    
        local label = gui.Label.create('赞助商')
        label:setfont(gui.Font.create(fontName, 20, "normal", "normal"))
        content:addchildview(label)

        local image = image:create('11对战平台', 'http://rpg.5211game.com/', fs.ydwe_devpath() / 'bin' / '11logo.png')
        content:addchildview(image)
        
        local function tool(name)
            local label = gui.Label.create(name)
            label:setcolor '#28c'
            label:setfont(gui.Font.create(fontName, 16, "normal", "normal"))
            content:addchildview(label)
        end
        local function author(name)
            local label = gui.Label.create(name)
            label:setfont(gui.Font.create(fontName, 12, "normal", "normal"))
            content:addchildview(label)
        end
    
        local label = gui.Label.create('感谢')
        label:setfont(gui.Font.create(fontName, 20, "normal", "normal"))
        content:addchildview(label)
    
        tool('YDWE')
        author('actboy168')
        author('sumneko')
        author('aeris')
        author('everguo')
        author('fetrix_sai')
        author('h82258652')
        author('shawwwn')
        author('Warft_TigerCN')
        author('xylign')
        
        tool('JassHelper')
        author('Vexorian')
    
        tool('Tesh')
        author('SFilip')
        author('Zoxc')
        author('Van Damm')
        author('wd40bomber7')
    
        tool('StormLib')
        author('Ladislav Zezula')
    
        tool('Pjass')
        author('Rudi Cilibrasi')
        author('lep')
    
        tool('Wave')
        author('Hartmut Kaiser')
    
        tool('Lua')
        author('Roberto Ierusalimschy')
        author('Waldemar Celes')
        author('Luiz Henrique de Figueiredo')
    
        tool('yue')
        author('zcbenz')
    end)    
end
