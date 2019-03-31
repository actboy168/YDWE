require 'filesystem'
local gui = require 'yue.gui'
local backend = require 'gui.backend'
local lang = require 'share.lang'
local ui = require 'gui.new.template'
local ev = require 'gui.event'
local root = fs.current_path()

local function count_report_height(text)
    local n = 1
    for _ in text:gmatch '\n' do
        n = n + 1
    end
    return n * 15
end

local template = ui.container {
    style = { FlexGrow = 1 },
    font = { size = 12 },
    ui.scroll {
        style = { FlexGrow = 1, Margin = 2 },
        hpolicy = 'never',
        vpolicy = 'never',
        width = 0,
        bind = {
            height = 'report.height'
        },
        ui.container {
            style = { FlexGrow = 1 },
            ui.label {
                style = { FlexGrow = 1 },
                text_color = '#CCC',
                align = 'start',
                bind = {
                    text = 'report.text'
                },
            },
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
                window:show_page('convert')
            end
        }
    }
}

local view, data = ui.create(template, {
    theme = window._color,
    report = {
        text = '',
        height = 0
    }
})

ev.on('update theme', function()
    data.theme = window._color
end)

function view:on_show()
    local text = io.load(root:parent_path() / 'log' / 'report.log') or ''
    data.report.text = text
    data.report.height = count_report_height(text)
    data.theme = window._color
end

return view
