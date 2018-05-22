local changelog = require 'share.changelog'
local window
local canvas
local current
local index
local page = 0
local color  = {
    NEW = {0, 173, 60},
    CHG = {217, 163, 60},
    FIX = {200, 30, 30},
    UI = {111, 77, 150},
}

return function(window_, canvas_)
    window = window_
    canvas = canvas_

    canvas:layout_row_dynamic(2, 1)
    canvas:layout_space(25, 10)
    index = 0

    if changelog[page * 5] then
        window:set_style('button.color', 81, 85, 97)
        canvas:layout_space_push(index, 0, 40, 25)
        index = index + 40
        if canvas:button('<<') then
            current = changelog[page * 5]
            page = page - 1
        end
    end

    for i = 1, 5 do
        local cl = changelog[page * 5 + i]
        if not cl then
            break
        end
        if not current then
            current = cl
        end
        if current == cl then
            window:set_style('button.color', 131, 135, 147)
            canvas:layout_space_push(index, 0, 80, 25)
            index = index + 80
        else
            window:set_style('button.color', 81, 85, 97)
            canvas:layout_space_push(index, 0, 60, 25)
            index = index + 60
        end
        if canvas:button(cl.version) then
            current = cl
        end
    end

    if changelog[page * 5 + 6] then
        window:set_style('button.color', 81, 85, 97)
        canvas:layout_space_push(index, 0, 40, 25)
        index = index + 40
        if canvas:button('>>') then
            current = changelog[page * 5 + 6]
            page = page + 1
        end
    end

    canvas:layout_row_dynamic(342, 1)
    canvas:group('说明', function()
        if current then
            for _, cl in ipairs(current) do
                local type, msg, tip = table.unpack(cl)
                window:set_style('button.color', table.unpack(color[type]))
                canvas:layout_space(25, 2)
                canvas:layout_space_push( 0, 0,  40, 25) canvas:button(type)
                canvas:layout_space_push(50, 0, 320, 25) canvas:text(msg, NK_TEXT_LEFT, tip)
            end
        end
    end)
end
