local gui = require 'yue.gui'
local backend = require 'gui.backend'
local timer = require 'gui.timer'
local messagebox = require 'ffi.messagebox'
local lang = require 'share.lang'
local config = require 'share.config'
local push_error = require 'gui.push_error'
local ui = require 'gui.new.template'
local databinding = require 'gui.new.databinding'
local ev = require 'gui.event'
require 'filesystem'

local root = fs.current_path()
local worker
local view
local data

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

local function update_show()
    data.report.visible = not not backend.lastword
    data.progress.visible = (not not worker) and not data.report.visible
end

local function update()
    worker:update()
    data.message = backend.message
    if backend.lastword then
        data.report.text = backend.lastword.content
        if backend.lastword.type == 'failed' or backend.lastword.type == 'error' then
            data.report.color = '#C33'
        elseif backend.lastword.type == 'warning' then
            data.report.color = '#FC3'
        else
            data.report.color = window._color
        end
    end
    data.progress.value = backend.progress / 100
    update_show()
    if worker.exited then
        if #worker.error > 0 then
            push_error(worker.error)
            worker.error = ''
            return 0, 1
        end
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
        messagebox(lang.ui.ERROR, '%s', r)
        return
    end
    if r then
        t:remove()
    end
end

local template = ui.container {
    style = { FlexGrow = 1, Padding = 4 },
    font = { size = 18 },
    -- upper
    ui.container {
        id = 'config',
        style = { FlexGrow = 1, JustifyContent = 'flex-start' },
        -- filename
        ui.button {
            style = { Height = 36, MarginTop = 4, MarginBottom = 16 },
            bind = {
                title = 'filename',
                color = 'theme'
            },
        }
    },
    -- lower
    ui.container {
        style = { FlexGrow = 1, JustifyContent = 'flex-end' },
        -- message
        ui.label {
            style = { Height = 20, MarginBottom = 8 },
            text_color = '#CCC',
            align = 'start',
            bind = {
                text = 'message',
            },
        },
        -- progress
        ui.progress {
            style = { Height = 30, MarginBottom = 8, FlexDirection = 'row' },
            bind = {
                value = 'progress.value',
                visible = 'progress.visible',
                color = 'theme'
            },
        },
        -- report
        ui.button {
            style = { Height = 30, MarginBottom = 8 },
            bind = {
                title = 'report.text',
                color = 'report.color',
                visible = 'report.visible',
            },
            on = {
                click = function ()
                    window:show_page 'report'
                end
            },
        },
        -- start
        ui.button {
            title = lang.ui.START,
            style = { Height = 50 },
            bind = {
                color = 'theme'
            },
            on = {
                click = function ()
                    if worker and not worker.exited then
                        return
                    end
                    backend:init(getexe(), fs.current_path())
                    worker = backend:open('backend\\init.lua', pack_arg())
                    backend.message = lang.ui.INIT
                    backend.progress = 0
                    data.progress.value = backend.progress / 100
                    data.progress.visible = true
                    data.report.visible = false
                    timer.loop(100, delayedtask)
                    window._worker = worker
                end,
            },
        },
    },
}

view, data, element = ui.create(template, {
    filename = '',
    message  = '',
    theme = window._color,
    report   = {
        text  = '',
        color = window._color,
        visible = false
    },
    progress = {
        value = 0,
        visible = false
    }
})

function view:on_show()
    update_show()
    data.filename = window._filename:filename():string()
end

local function checkbox(t)
    t.on = {
        mouseenter = 'update_tip(self.tip)',
        mouseleave = 'update_tip()'
    }
    t.style = { MarginTop = 4, MarginBottom = 4 }
    if t.bind then
        t.bind.color = 'theme'
    else
        t.bind = { color = 'theme' }
    end
    return ui.checkbox(t)
end

local configData = databinding {
    theme = window._color,
    config = config,
    update_tip = function(tip)
        if worker and not worker.exited then
            return
        end
        if tip then
            data.message = tip
        else
            data.message = ''
        end
    end
}

local function lni()
    local template = ui.container {
        font = { size = 18 },
        checkbox {
            text = lang.ui.READ_SLK,
            tip = lang.ui.READ_SLK_HINT,
            bind = {
                value = 'config.lni.read_slk'
            }
        },
        ui.tree {
            text = lang.ui.ADVANCED,
            style = { MarginTop = 4, MarginBottom = 4 },
            bind = {
                color = 'theme'
            },
            checkbox {
                text = lang.ui.EXPORT_LUA,
                tip = lang.ui.EXPORT_LUA_HINT,
                bind = {
                    value = 'config.lni.export_lua'
                }
            }
        }
    }
    return ui.createEx(template, configData)
end

local function slk()
    local template = ui.container {
        font = { size = 18 },
        checkbox {
            text = lang.ui.REMOVE_UNUSED_OBJECT,
            tip = lang.ui.REMOVE_UNUSED_OBJECT_HINT,
            bind = {
                value = 'config.slk.remove_unuse_object'
            }
        },
        checkbox {
            text = lang.ui.OPTIMIZE_JASS,
            tip = lang.ui.OPTIMIZE_JASS_HINT,
            bind = {
                value = 'config.slk.optimize_jass'
            }
        },
        checkbox {
            text = lang.ui.MDX_SQUF,
            tip = lang.ui.MDX_SQUF_HINT,
            bind = {
                value = 'config.slk.mdx_squf'
            }
        },
        checkbox {
            text = lang.ui.REMOVE_WE_ONLY,
            tip = lang.ui.REMOVE_WE_ONLY_HINT,
            bind = {
                value = 'config.slk.remove_we_only'
            }
        },
        ui.tree {
            text = lang.ui.ADVANCED,
            style = { MarginTop = 4, MarginBottom = 4 },
            bind = {
                color = 'theme'
            },
            checkbox {
                text = lang.ui.SLK_DOODAD,
                tip = lang.ui.SLK_DOODAD_HINT,
                bind = {
                    value = 'config.slk.slk_doodad'
                }
            },
            checkbox {
                text = lang.ui.CONFUSION,
                tip = lang.ui.CONFUSION_HINT,
                bind = {
                    value = 'config.slk.confused'
                }
            }
        }
    }
    return ui.createEx(template, configData)
end

local function obj()
    local template = ui.container {
        font = { size = 18 },
        checkbox {
            text = lang.ui.READ_SLK,
            tip = lang.ui.READ_SLK_HINT
        }
    }
    return ui.createEx(template, configData)
end

local function mpq()
    return ui.create(ui.container { }, {})
end

local current_page
local pages = {}

ev.on('update theme', function(color, title)
    data.theme = color
    data.report.color = color
    backend.lastword = nil
    data.message = ''
    worker = nil
    
    configData.proxy.theme = color

    if current_page then
        current_page:setvisible(false)
    end
    if not pages[title] then
        if title == 'W3x2Lni' then
            pages[title] = lni()
        elseif title == 'W3x2Slk' then
            pages[title] = slk()
        elseif title == 'W3x2Obj' then
            pages[title] = obj()
        elseif title == 'War3Dump' then
            pages[title] = mpq()
        end
        element.config:addchildview(pages[title])
    end
    current_page = pages[title]
    current_page:setvisible(true)
end)

return view
