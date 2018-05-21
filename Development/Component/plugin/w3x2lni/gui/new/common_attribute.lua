local gui = require 'yue.gui'

local function BindValue(t, data, bind, name, func)
    if t.bind and t.bind[name] then
        bind[name] = data:bind(t.bind[name], function()
            func(bind[name]:get())
        end)
        func(bind[name]:get())
    else
        if t[name] ~= nil then
            func(t[name])
        end
    end
end

local function GetHoverColor(color)
    if #color == 4 then
        return ('#%01X%01X%01X'):format(
            math.min(tonumber(color:sub(2, 2), 16) + 0x1, 0xF),
            math.min(tonumber(color:sub(3, 3), 16) + 0x1, 0xF),
            math.min(tonumber(color:sub(4, 4), 16) + 0x1, 0xF)
        )
    elseif #color == 7 then
        return ('#%02X%02X%02X'):format(
            math.min(tonumber(color:sub(2, 3), 16) + 0x10, 0xFF),
            math.min(tonumber(color:sub(4, 5), 16) + 0x10, 0xFF),
            math.min(tonumber(color:sub(6, 7), 16) + 0x10, 0xFF)
        )
    else
        return color
    end
end

local function label_color(self, t, data, bind)
    local color_hover = ''
    local color_normal = ''
    local event = false
    BindValue(t, data, bind, 'color', function (color)
        color_normal = color
        self:setbackgroundcolor(color_normal)
    end)
    BindValue(t, data, bind, 'color_hover', function (color)
        color_hover = color
        if not event then
            event = true
            function self:onmouseleave()
                self:setbackgroundcolor(color_normal)
            end
            function self:onmouseenter()
                self:setbackgroundcolor(color_hover)
            end
        end
    end)
end

local function button_color(self1, self2, t, data, bind)
    local color_hover = ''
    local color_normal = ''
    local has_hover = false
    local event = false
    BindValue(t, data, bind, 'color', function (color)
        color_normal = color
        if not has_hover then
            color_hover = GetHoverColor(color)
        end
        if not event then
            event = true
            function self1:onmouseleave()
                self2:setbackgroundcolor(color_normal)
            end
            function self1:onmouseenter()
                self2:setbackgroundcolor(color_hover)
            end
        end
        self2:setbackgroundcolor(color_normal)
    end)
    BindValue(t, data, bind, 'color_hover', function (color)
        color_hover = true
        color_hover = color
        if not event then
            event = true
            function self1:onmouseleave()
                self2:setbackgroundcolor(color_normal)
            end
            function self1:onmouseenter()
                self2:setbackgroundcolor(color_hover)
            end
        end
    end)
end

local function visible(self, t, data, bind)
    BindValue(t, data, bind, 'visible', function (value)
        self:setvisible(value)
    end)
end

local fontPool = {}
local fontDefault = gui.app:getdefaultfont()
local fontName = fontDefault:getname()
local fontSize = fontDefault:getsize()
local function font(self, t)
    if not t.font then
        return
    end
    local name = t.font.name or fontName
    local size = t.font.size or fontSize
    local weight = t.font.weight or 'normal'
    local style = t.font.style or 'normal'
    local key = ('%s|%d|%s|%s'):format(name, size, weight, style)
    local r = fontPool[key]
    if not r then
        r = gui.Font.create(name, size, weight, style)
        fontPool[key] = r
    end
    self:setfont(r)
end

local function event(self, t, data, name)
    if not t.on or not t.on[name] then
        return
    end
    local on = t.on[name]
    if type(on) == 'function' then
        self['on'..name] = function (self)
            on(t)
        end
    elseif type(on) == 'string' then
        self['on'..name] = function (self)
            data:execute(on, t)
        end
    end
end

return {
    label_color = label_color,
    button_color = button_color,
    visible = visible,
    font = font,
    event = event,
}
