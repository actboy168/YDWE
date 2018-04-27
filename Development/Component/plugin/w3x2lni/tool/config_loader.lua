require 'filesystem'
local lang = require 'tool.lang'
local root = fs.current_path()

local function proxy(t)
    local keys = {}
    local mark = {}
    local func = {}
    local value = {}
    local comment = {}
    local fmt = {}
    return setmetatable(t, {
        __index = function (_, k)
            return value[k]
        end,
        __newindex = function (_, k, v)
            if type(v) == 'function' then
                func[k] = v
            elseif func[k] then
                local suc, res1, res2 = func[k](v)
                if suc then
                    value[k], fmt[k] = res1, res2
                end
            elseif type(v) == 'table' then
                if next(v) then
                    func[k] = v[1]
                    comment[k] = v[2]
                else
                    value[k] = proxy(v)
                end
            end
            if not mark[k] then
                mark[k] = true
                keys[#keys+1] = k
            end
        end,
        __pairs = function ()
            local i = 0
            return function ()
                i = i + 1
                local k = keys[i]
                return k, value[k], fmt[k], func[k], comment[k]
            end
        end,
    })
end

local function string(v)
    v = tostring(v)
    local r = v
    if r:find '%c' or r:find '^[%-%d%.]' or r == 'nil' or r == 'true' or r == 'false' or r == '' then
        r = '"' .. r:gsub('"', '\\"'):gsub('\r', '\\r'):gsub('\n', '\\n') .. '"'
    end
    return true, v, r
end

local function boolean(v)
    if type(v) == 'boolean' then
        return true, v, tostring(v)
    elseif v == 'true' then
        return true, true, 'true'
    elseif v == 'false' then
        return true, false, 'false'
    else
        return false, lang.raw.CONFIG_MUST_BOOLEAN
    end
end

local function integer(v)
    local v = math.tointeger(v)
    if v then
        return true, v, tostring(v)
    else
        return false, lang.raw.CONFIG_MUST_INTEGER
    end
end

local function confusion(confusion)
    if not string(confusion) then
        return string(confusion)
    end

    if confusion:find '[^%w_]' then
        return false, lang.raw.CONFIG_CONFUSION_1
    end
    
    local chars = {}
    for char in confusion:gmatch '[%w_]' do
        if not chars[char] then
            chars[#chars+1] = char
        end
    end
    if #chars < 3 then
        return false, lang.raw.CONFIG_CONFUSION_2
    end
    
    confusion = table.concat(chars)

    local count = 0
    for _ in confusion:gmatch '%a' do
        count = count + 1
    end
    if count < 2 then
        return false, lang.raw.CONFIG_CONFUSION_3
    end

    return string(confusion)
end

local function get_langs()
    local locale = root / 'locale'
    local list = {}
    for dir in locale:list_directory() do
        if fs.is_directory(dir) then
            list[#list+1] = dir:filename():string()
        end
    end
    table.sort(list)
    return list
end

local function insert_lang(chars, max, lng)
    chars[#chars+1] = '        '
    chars[#chars+1] = lng
    chars[#chars+1] = (' '):rep(max + 1 - #lng)
    if lng == '${AUTO}' then
        chars[#chars+1] = lang.raw.AUTO_SELECT
    else
        chars[#chars+1] = io.load(root / 'locale' / lng / 'name')
    end
    chars[#chars+1] = '\r\n'
end

local function lang_hint()
    local list = get_langs()
    local max = #'${AUTO}'
    for _, lang in ipairs(list) do
        if #lang > max then
            max = #lang
        end
    end

    local chars = {}
    insert_lang(chars, max, '${AUTO}')
    for _, lang in ipairs(list) do
        insert_lang(chars, max, lang)
    end
    return table.concat(chars)
end

local function langf(v)
    if v == '${AUTO}' then
        return true, v, v
    end
    local list = get_langs()
    for _, lang in ipairs(list) do
        if lang == v then
            return true, v, v
        end
    end
    return false, lang.raw.CONFIG_GLOBAL_LANG_ERROR .. lang_hint()
end

local function get_datas()
    local locale = root:parent_path() / 'data'
    local list = {}
    for dir in locale:list_directory() do
        if fs.is_directory(dir) then
            list[#list+1] = dir:filename():string()
        end
    end
    table.sort(list)
    return list
end

local function data_hint()
    local list = get_datas()
    local chars = {}
    for _, data in ipairs(list) do
        chars[#chars+1] = '        '
        chars[#chars+1] = data
        chars[#chars+1] = '\r\n'
    end
    return table.concat(chars)
end

local function war3(v)
    local list = get_datas()
    for _, name in ipairs(list) do
        if name == v then
            return true, v, v
        end
    end
    return false, lang.raw.CONFIG_GLOBAL_WAR3_ERROR .. data_hint()
end

local function ui(v)
    if v == '${YDWE}' then
        return true, v, v
    end
    local list = get_datas()
    for _, name in ipairs(list) do
        if name == v then
            return true, v, v
        end
    end
    return false, lang.raw.CONFIG_GLOBAL_UI_ERROR .. data_hint()
end

local function meta(v)
    if v == '${DEFAULT}' then
        return true, v, v
    end
    local list = get_datas()
    for _, name in ipairs(list) do
        if name == v then
            return true, v, v
        end
    end
    return false, lang.raw.CONFIG_GLOBAL_META_ERROR .. data_hint()
end

local function wes(v)
    if v == '${YDWE}' then
        return true, v, v
    end
    local list = get_datas()
    for _, name in ipairs(list) do
        if name == v then
            return true, v, v
        end
    end
    return false, lang.raw.CONFIG_GLOBAL_WES_ERROR .. data_hint()
end

local function string_proxy(key, concat)
    return setmetatable({}, {
        __tostring = function ()
            if concat then
                return lang.raw[key] .. concat()
            else
                return lang.raw[key]
            end
        end,
        __concat = function (_, concat)
            return string_proxy(key, concat)
        end,
    })
end

local raw = setmetatable({}, {
    __index = function (_, key)
        return string_proxy(key)
    end,
})

return function ()
    local config = proxy {}
    
    config.global                  = {}
    config.global.lang             = {langf,  raw.CONFIG_GLOBAL_LANG .. lang_hint}
    config.global.data_war3        = {war3,   raw.CONFIG_GLOBAL_WAR3 .. data_hint}
    config.global.data_ui          = {ui,     raw.CONFIG_GLOBAL_UI   .. data_hint}
    config.global.data_meta        = {meta,   raw.CONFIG_GLOBAL_META .. data_hint}
    config.global.data_wes         = {wes,    raw.CONFIG_GLOBAL_WES  .. data_hint}

    config.lni                     = {}
    config.lni.read_slk            = {boolean, raw.CONFIG_LNI_READ_SLK}
    config.lni.find_id_times       = {integer, raw.CONFIG_LNI_FIND_ID_TIMES}
    config.lni.export_lua          = {boolean, raw.CONFIG_LNI_EXPORT_LUA}

    config.slk                     = {}
    config.slk.remove_unuse_object = {boolean,   raw.CONFIG_LNI_REMOVE_UNUSE_OBJECT}
    config.slk.optimize_jass       = {boolean,   raw.CONFIG_SLK_OPTIMIZE_JASS}
    config.slk.mdx_squf            = {boolean,   raw.CONFIG_SLK_MDX_SQUF}
    config.slk.remove_we_only      = {boolean,   raw.CONFIG_SLK_REMOVE_WE_ONLY}
    config.slk.slk_doodad          = {boolean,   raw.CONFIG_SLK_SLK_DOODAD}
    config.slk.find_id_times       = {integer,   raw.CONFIG_SLK_FIND_ID_TIMES}
    config.slk.confused            = {boolean,   raw.CONFIG_SLK_CONFUSED}
    config.slk.confusion           = {confusion, raw.CONFIG_SLK_CONFUSION}

    config.obj                     = {}
    config.obj.read_slk            = {boolean, raw.CONFIG_OBJ_READ_SLK}
    config.obj.find_id_times       = {integer, raw.CONFIG_OBJ_FIND_ID_TIMES}

    return config
end
