local lang = require 'lang'
local w2l
local hex
local unpack_pos
local doo

local string_unpack = string.unpack
local select = select

local function set_pos(...)
    unpack_pos = select(-1, ...)
    return ...
end

local function unpack(str)
    return set_pos(string_unpack(str, unpack_buf, unpack_pos))
end

local function unpack_head()
    local flag = unpack 'c4'
    if flag ~= 'W3do' then
        error(lang.report.UNSUPPORT_FILE_TYPE..flag)
    end
    local version = unpack 'l'
    if version ~= 8 then
        error(lang.report.UNSUPPORT_VERSION..version)
    end
    local unknow = unpack 'l'
    if unknow ~= 11 then
        error(lang.report.UNSUPPORT_UNKNOWN_VALUE..unknow)
    end
end

local function unpack_obj()
    local id = unpack 'c4'
    local obj = {
        ['样式'] = unpack '<l',
        ['位置'] = {unpack 'fff'},
        ['角度'] = math.deg(unpack 'f'),
        ['缩放'] = {unpack 'fff'},
        ['可见'] = unpack 'I1',
        ['生命'] = unpack 'I1',
    }

    obj['掉落列表'] = unpack 'l'
    local count = unpack 'l'
    if count > 0 then
        obj['掉落'] = {}
        for i = 1, count do
            obj['掉落'][i] = {
                ['ID'] = unpack 'c4',
                ['几率'] = unpack 'l'
            }
        end
    end

    obj['编号'] = unpack 'l'

    doo[id] = obj
end

local function unpack_objs()
    local count = unpack 'l'

    for i = 1, count do
        unpack_obj()
    end
end

local function unpack_special()
    local id = unpack 'l'
    obj[id] = {
        ['未知'] = unpack 'l',
        ['位置'] = {unpack 'll'},
    }
end

local function unpack_specials()
    local count = unpack 'l'

    for i = 1, count do
        unpack_special()
    end
end

local function unpack_special_head()
    local version = unpack 'l'
    if version ~= 0 then
        error(lang.report.UNSUPPORT_VERSION..version)
    end
end

return function (w2l_, hex_)
    w2l = w2l_
    hex = hex_
    unpack_pos = 1

    doo = {}

    unpack_head()
    unpack_objs()

    unpack_special_head()
    unpack_specials()

    return doo
end
