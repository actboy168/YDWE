local gui = require 'yue.gui'
local backend = require 'gui.backend'
local timer = require 'gui.timer'
local messagebox = require 'ffi.messagebox'
local lang = require 'tool.lang'
require 'filesystem'

local worker
local view
local pb
local label
local lower
local report

local function getexe()
    local i = 0
    while arg[i] ~= nil do
        i = i - 1
    end
    return fs.path(arg[i + 1])
end

local function pack_arg()
    local buf = {}
    buf[1] = window._mode
    buf[2] = '"' .. window._filename:string() .. '"'
    return table.concat(buf, ' ')
end

local function update_progress(value)
    pb:set_progress(value)
end

local function update_show()
    if backend.lastword then
        report:setvisible(true)
    else
        report:setvisible(false)
    end
    if worker and not report:isvisible() then
        pb:setvisible(true)
    else
        pb:setvisible(false)
    end
end

local function update()
    worker:update()
    message:settext(backend.message)
    if backend.lastword then
        report:settitle(backend.lastword.content)
        if backend.lastword.type == 'failed' or backend.lastword.type == 'error' then
            report:update_color('#C33')
        elseif backend.lastword.type == 'warning' then
            report:update_color('#FC3')
        else
            report:update_color()
        end
    end
    update_progress(backend.progress)
    update_show()
    if #worker.error > 0 then
        messagebox(lang.ui.ERROR, worker.error)
        worker.error = ''
        return 0, 1
    end
    if worker.exited then
        if worker.exit_code == 0 then
            return 1000, 0
        else
            return 0, worker.exit_code
        end
    end
end

local function delayedtask(t)
    local ok, r, code = xpcall(update, debug.traceback)
    if not ok then
        t:remove()
        messagebox(lang.ui.ERROR, r)
        return
    end
    if r then
        t:remove()
    end
end

view = gui.Container.create()
view:setstyle { FlexGrow = 1, Padding = 2 }

local upper = gui.Container.create()
upper:setstyle { FlexGrow = 1, JustifyContent = 'flex-start' }
view:addchildview(upper)

lower = gui.Container.create()
lower:setstyle { FlexGrow = 1, JustifyContent = 'flex-end' }
view:addchildview(lower)

local filename = Button('')
filename:setstyle { Height = 50, Margin = 2 }
filename:setfont(Font('黑体', 20))
upper:addchildview(filename)

message = gui.Label.create('')
message:setstyle { Height = 20, Margin = 2 }
message:setfont(Font('黑体', 20))
message:setcolor('#CCC')
message:setalign('start')
lower:addchildview(message)

pb = Progress()
pb:setstyle { Height = 30, Margin = 5, Padding = 3, FlexDirection = 'row' }
lower:addchildview(pb)

report = Button('')
report:setstyle { Height = 30, Margin = 5 }
report:setfont(Font('黑体', 24))
lower:addchildview(report)

local start = Button('开始')
start:setstyle { Height = 50, Margin = 2 }
start:setfont(Font('黑体', 24))
lower:addchildview(start)

function start:onclick()
    if worker and not worker.exited then
        return
    end
    pb:setvisible(true)
    report:setvisible(false)
    backend:init(getexe(), fs.current_path())
    worker = backend:open('backend\\init.lua', pack_arg())
    backend.message = lang.ui.INIT
    backend.progress = 0
    update_progress(backend.progress)
    timer.loop(100, delayedtask)
    window._worker = worker
end

function report:onclick()
    if next(backend.report) then
        window:show_page 'report'
        window:show_report()
    end
end

function view:on_show()
    update_show()
    filename:settitle(window._filename:filename():string())
    filename:update_color()
    start:update_color()
    pb:update_color()
end

return view
