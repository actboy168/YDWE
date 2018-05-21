local lang = require 'share.lang'
local ui = require 'gui.new.template'
local ev = require 'gui.event'

local template = ui.container {
    style = { FlexGrow = 1 },
    font = { size = 16 },
    ui.container {
        style = { FlexGrow = 1 },
        ui.label {
            text = lang.ui.AUTHOR,
            text_color = '#000',
            style = { MarginTop = 20, Height = 28, Width = 240 },
            bind = {
                color = 'theme'
            }
        },
        ui.label {
            text = lang.ui.FRONTEND .. 'actboy168',
            text_color = '#AAA',
            style = { MarginTop = 5, Height = 28, Width = 240 }
        },
        ui.label {
            text = lang.ui.BACKEND .. lang.ui.SUMNEKO,
            text_color = '#AAA',
            style = { Height = 28, Width = 240 }
        },
        ui.label {
            text = lang.ui.CHANGE_LOG,
            text_color = '#000',
            style = { Height = 28, Width = 240 },
            bind = {
                color = 'theme'
            }
        },
        ui.scroll {
            style = { FlexGrow = 1 },
            hpolicy = 'never',
            vpolicy = 'never',
            width = 0,
            bind = {
                height = 'height'
            },
            ui.container {
                id = 'changelog',
            }
        },
    },
    ui.button {
        title = lang.ui.BACK,
        style = { Bottom = 0, Height = 28, Margin = 5 },
        bind = {
            color = 'theme'
        },
        on = {
            click = function()
                window:show_page('index')
            end
        }
    }
}

local template_version = ui.label {
    style = { Margin = 3, Height = 25 },
    color = '#444',
    text_color = '#AAA',
    font = { size = 16 },
    bind = {
        text = 'version'
    }
}

local template_changelog = ui.container {
    style = { Height = 31, FlexDirection = 'row' },
    color = '#222',
    color_hover = '#444',
    ui.label {
        style = { Margin = 3, Width = 40 },
        font = { name = 'Consolas', size = 18 },
        bind = {
            text = 'type.text',
            color = 'type.color'
        }
    },
    ui.label {
        style = { Margin = 3, Width = 360, FlexGlow = 1 },
        text_color = '#AAA',
        font = { size = 16 },
        align = 'start',
        bind = {
            text = 'text'
        }
    }
}

local color  = {
    NEW = '#00AD3C',
    CHG = '#D9A33C',
    FIX = '#C81E1E',
    UI =  '#6F4D96',
}

local view, data, element = ui.create(template, {
    theme = window._color,
    height = 0
})

local log = element.changelog
local height = 0
for _, v in ipairs(require 'share.changelog') do
    local version = ui.create(template_version, {
        version = v.version
    })
    log:addchildview(version)
    height = height + 31

    for _, l in ipairs(v) do
        local changelog = ui.create(template_changelog, {
            type = {
                text = l[1],
                color = color[l[1]]
            },
            text = l[2]
        })
        log:addchildview(changelog)
        height = height + 31
    end
end

data.height = height
ev.on('update theme', function()
    data.theme = window._color
end)

return view
