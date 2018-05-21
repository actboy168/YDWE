local lang = require 'share.lang'
local ui = require 'gui.new.template'
local ev = require 'gui.event'

local template = ui.container {
    style = { FlexGrow = 1, Padding = 1 },
    ui.container{
        font = { size = 18 },
        style = { Padding = 4, JustifyContent = 'flex-start' },
        ui.button {
            style = { Height = 36, MarginTop = 4 },
            bind = {
                title = 'filename',
                color = 'theme'
            }
        }
    },
    ui.container {
        font = { size = 20 },
        style =  { Padding = 4, FlexGrow = 1, JustifyContent = 'flex-end' },
        ui.button {
            title = lang.ui.CONVERT_TO..'Lni',
            color = '#00ADD9',
            style = { MarginTop = 1, MarginBottom = 1, Height = 155 },
            on = {
                click = function()
                    window._mode = 'lni'
                    window:set_theme('W3x2Lni', '#00ADD9')
                    window:show_page 'convert'
                    window:set_theme('W3x2Lni', '#00ADD9')
                end
            }
        },
        ui.button {
            title = lang.ui.CONVERT_TO..'Slk',
            color = '#00AD3C',
            style = { MarginTop = 1, MarginBottom = 1, Height = 155 },
            on = {
                click = function()
                    window._mode = 'slk'
                    window:set_theme('W3x2Slk', '#00AD3C')
                    window:show_page 'convert'
                    window:set_theme('W3x2Slk', '#00AD3C')
                end
            }
        },
        ui.button {
            title = lang.ui.CONVERT_TO..'Obj',
            color = '#D9A33C',
            style = { MarginTop = 1, MarginBottom = 1, Height = 155 },
            on = {
                click = function()
                    window._mode = 'obj'
                    window:set_theme('W3x2Obj', '#D9A33C')
                    window:show_page 'convert'
                    window:set_theme('W3x2Obj', '#D9A33C')
                end
            }
        }
    }
}

local view, data = ui.create(template, {
    filename = '',
    theme = window._color
})

ev.on('update theme', function()
    data.theme = window._color
end)

function view:on_show()
    window:set_theme('W3x2Lni', '#00ADD9')
    data.filename = window._filename:filename():string()
end

return view
