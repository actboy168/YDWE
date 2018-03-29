local table_concat = table.concat

local current_x, current_y
local function get_data(x, y, k)
    local str = {}
    str[#str+1] = 'C'
    if x ~= current_x then
        current_x = x
        str[#str+1] = ('X%d'):format(x)
    end
    if y ~= current_y then
        current_y = y
        str[#str+1] = ('Y%d'):format(y)
    end
    if tonumber(k) then
        str[#str+1] = ('K%s'):format(k)
    else
        str[#str+1] = ('K"%s"'):format(k)
    end
    return table_concat(str, ';')
end

local current_titles
local function insert_title(buf, title)
    local x = #current_titles+1
    current_titles[x] = title
    current_titles[title] = x
    buf[#buf+1] = get_data(x, 1, title)
    return x
end

local function get_title(buf, title)
    return current_titles[title] or insert_title(buf, title)
end

local function insert_obj(buf, tbl)
    local y = 1
    for name, obj in pairs(tbl) do
        y = y + 1
        buf[#buf+1] = get_data(1, y, name)
        for key, value in pairs(obj) do
            local str = get_data(get_title(buf, key), y, value)
            buf[#buf+1] = str
        end
    end
    return y
end

return function(t, id_title)
    local buf = {'ID;PWXL;N;E', ''}
    current_x = nil
    current_y = nil
    current_titles = {}
    insert_title(buf, id_title)
    local y = insert_obj(buf, t)
    buf[#buf+1] = 'E'
    buf[2] = ('B;X%d;Y%d;D%d'):format(#current_titles, y, 0)
    return table_concat(buf, '\r\n')
end
