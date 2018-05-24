require 'filesystem'
require 'utility'
local messagebox = require 'ffi.messagebox'
local process = require 'process'
local nk = require 'nuklear'
local backend = require 'gui.backend'
local show_version = require 'gui.old.show_version'
local plugin = require 'gui.old.plugin'
local setting = require 'share.setting'
local lang = require 'share.lang'
local input_path = require 'share.input_path'
local builder = require 'map-builder'
local war3 = require 'share.war3'
local push_error = require 'gui.push_error'
local currenttheme = {0, 173, 217}
local worker

NK_WIDGET_STATE_MODIFIED = 1 << 1
NK_WIDGET_STATE_INACTIVE = 1 << 2
NK_WIDGET_STATE_ENTERED  = 1 << 3
NK_WIDGET_STATE_HOVER    = 1 << 4
NK_WIDGET_STATE_ACTIVED  = 1 << 5
NK_WIDGET_STATE_LEFT     = 1 << 6

NK_TEXT_ALIGN_LEFT     = 0x01
NK_TEXT_ALIGN_CENTERED = 0x02
NK_TEXT_ALIGN_RIGHT    = 0x04
NK_TEXT_ALIGN_TOP      = 0x08
NK_TEXT_ALIGN_MIDDLE   = 0x10
NK_TEXT_ALIGN_BOTTOM   = 0x20
NK_TEXT_LEFT           = NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_LEFT
NK_TEXT_CENTERED       = NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_CENTERED
NK_TEXT_RIGHT          = NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_RIGHT

local root = fs.current_path():remove_filename()
local fmt = nil
local setting

local function getexe()
	local i = 0
	while arg[i] ~= nil do
		i = i - 1
	end
	return fs.path(arg[i + 1])
end

backend:init(getexe(), root / 'script')
local window = nk.window('W3x2Lni', 400, 600)
window:set_theme(0, 173, 217)

local uitype = 'none'
local showmappath = false
local showcount = 0
local mapname = ''
local mappath = fs.path()

function window:dropfile(file)
    if worker and not worker.exited then
        return
    end
    if war3:open(fs.path(file)) then
        return
    end
    local inputpath = input_path(file)
    local map = builder.load(inputpath)
    if not map then
        return
    end
    map:close()
    mappath = inputpath
    mapname = mappath:filename():string()
    setting:open_map(mappath)
    uitype = 'select'
end

local function set_current_theme(theme)
    if theme then
        currenttheme = theme
    end
    window:set_theme(table.unpack(currenttheme))
end

local function reset_button_color()
    window:set_style('button.color', table.unpack(currenttheme))
end

local function button_mapname(canvas, height)
    canvas:layout_row_dynamic(10, 1)
    canvas:layout_row_dynamic(40, 1)
    local ok, state = canvas:button(mapname)
    if ok then 
        showmappath = not showmappath
    end
    if state & NK_WIDGET_STATE_LEFT ~= 0 then
        showcount = showcount + 1
    else
        showcount = 0
    end
    if showmappath or showcount > 20 then
        canvas:edit(mappath:string(), 200, function ()
            return false
        end)
        height = height - 44
    end
    return height
end

local version = (require 'share.changelog')[1].version
local function button_about(canvas)
    window:set_style('button.color', 51, 55, 67)
    canvas:text('', NK_TEXT_RIGHT)
    if canvas:button(lang.ui.VERSION .. version) then
        uitype = 'about'
    end
end

local show_plugin

local function button_plugin(canvas)
    show_plugin = plugin()
    canvas:text('', NK_TEXT_RIGHT)
    if show_plugin then
        window:set_style('button.color', 51, 55, 67)
        if canvas:button(lang.ui.PLUGIN) then
            uitype = 'plugin'
        end
    else
        canvas:text('', NK_TEXT_RIGHT)
    end
end

local function window_plugin(canvas)
    canvas:layout_row_dynamic(20, 1)
    canvas:layout_space(30, 1)
    canvas:layout_space_push(-10, 0, 300, 30)
    canvas:button(lang.ui.PLUGIN)
    if show_plugin then
        show_plugin(canvas)
    else
        if mapname == '' then
            uitype = 'none'
        else
            uitype = 'select'
        end
    end
    canvas:layout_row_dynamic(30, 1)
    if canvas:button(lang.ui.BACK) then
        if mapname == '' then
            uitype = 'none'
        else
            uitype = 'select'
        end
    end
end

local function window_about(canvas)
    canvas:layout_row_dynamic(20, 1)
    canvas:layout_space(30, 1)
    canvas:layout_space_push(-10, 0, 300, 30)
    canvas:button(lang.ui.AUTHOR)
    canvas:layout_row_dynamic(5, 1)
    canvas:layout_row_dynamic(20, 4)
    canvas:text(lang.ui.FRONTEND, NK_TEXT_RIGHT) canvas:text('actboy168', NK_TEXT_CENTERED) 
    canvas:layout_row_dynamic(20, 4)
    canvas:text(lang.ui.BACKEND, NK_TEXT_RIGHT) canvas:text(lang.ui.SUMNEKO, NK_TEXT_CENTERED)
    canvas:layout_row_dynamic(5, 1)
    canvas:layout_space(30, 1)
    canvas:layout_space_push(-10, 0, 300, 30)
    canvas:button(lang.ui.INTRUCT)
    show_version(window, canvas)
    reset_button_color()
    canvas:layout_row_dynamic(30, 1)
    if canvas:button(lang.ui.BACK) then
        if mapname == '' then
            uitype = 'none'
        else
            uitype = 'select'
        end
    end
end

local function window_none(canvas)
    canvas:layout_row_dynamic(2, 1)
    canvas:layout_row_dynamic(200, 1)
    canvas:button(lang.ui.DRAG_MAP)
    canvas:layout_row_dynamic(290, 1)
    canvas:layout_row_dynamic(20, 2)
    button_plugin(canvas)
    button_about(canvas)
    set_current_theme()
end

local function clean_convert_ui()
    backend:clean()
end

local function window_select(canvas)
    canvas:layout_row_dynamic(2, 1)
    canvas:layout_row_dynamic(100, 1)
    window:set_theme(0, 173, 217)
    if canvas:button(lang.ui.TO_LNI) then
        uitype = 'convert'
        fmt = 'lni'
        window:set_title('W3x2Lni')
        clean_convert_ui()
        set_current_theme {0, 173, 217}
        return
    end
    window:set_theme(0, 173, 60)
    if canvas:button(lang.ui.TO_SLK) then
        uitype = 'convert'
        fmt = 'slk'
        window:set_title('W3x2Slk')
        clean_convert_ui()
        set_current_theme {0, 173, 60}
        return
    end
    window:set_theme(217, 163, 60)
    if canvas:button(lang.ui.TO_OBJ) then
        uitype = 'convert'
        fmt = 'obj'
        window:set_title('W3x2Obj')
        clean_convert_ui()
        set_current_theme {217, 163, 60}
        return
    end
    canvas:layout_row_dynamic(182, 1)
    canvas:layout_row_dynamic(20, 2)
    button_plugin(canvas)
    button_about(canvas)
    set_current_theme()
end

local function update_worker()
    if worker then
        worker:update()
        if #worker.error > 0 then
            push_error(worker.error)
            worker.error = ''
        end
    end
end

local current_tip

local function checkbox_tip(canvas, text, tip, active)
    local ok, _, state = canvas:checkbox(text, active)
    if state & NK_WIDGET_STATE_LEFT ~= 0 then
        current_tip = tip
    end
    return ok
end

local function checkbox_simple(canvas, text, tip, data)
    if checkbox_tip(canvas, text, tip, setting[fmt][data]) then
        setting[fmt][data] = not setting[fmt][data]
    end
end

local function window_convert(canvas)
    current_tip = nil
    local height = button_mapname(canvas, 290)
    canvas:layout_row_dynamic(10, 1)
    if fmt == 'lni' or fmt == 'obj' then
        height = height - 24
        canvas:layout_row_dynamic(30, 1)
        checkbox_simple(canvas, lang.ui.READ_SLK, lang.ui.READ_SLK_HINT, 'read_slk')
    else
        height = height - 129
        canvas:layout_row_dynamic(30, 1)
        checkbox_simple(canvas, lang.ui.REMOVE_UNUSED_OBJECT, lang.ui.SIMPLIFY_HINT, 'remove_unuse_object')
        checkbox_simple(canvas, lang.ui.OPTIMIZE_JASS, lang.ui.OPTIMIZE_JASS_HINT, 'optimize_jass')
        checkbox_simple(canvas, lang.ui.MDX_SQUF, lang.ui.MDX_SQUF_HINT, 'mdx_squf')
        checkbox_simple(canvas, lang.ui.REMOVE_WE_ONLY, lang.ui.REMOVE_WE_ONLY_HINT, 'remove_we_only')
    end
    canvas:layout_row_dynamic(10, 1)
    canvas:tree(lang.ui.ADVANCED, 1, function()
        canvas:layout_row_dynamic(30, 1)
        if fmt == 'slk' then
            checkbox_simple(canvas, lang.ui.SLK_DOODAD, lang.ui.SLK_DOODAD_HINT, 'slk_doodad')
            height = height - 34
        end
        if fmt == 'lni' then
            checkbox_simple(canvas, lang.ui.EXPORT_LUA, lang.ui.EXPORT_LUA_HINT, 'export_lua')
            height = height - 34
        end
        if checkbox_tip(canvas, lang.ui.FIND_ID_TIMES, lang.ui.FIND_ID_TIMES_HINT, setting[fmt].find_id_times ~= 0) then
            if setting[fmt].find_id_times == 0 then
                setting[fmt].find_id_times = 10
            else
                setting[fmt].find_id_times = 0
            end
        end
        height = height - 34
        if setting[fmt].find_id_times ~= 0 then
            local r = canvas:edit(tostring(setting[fmt].find_id_times), 10, function (c)
                return 48 <= c and c <= 57
            end)
            local n = tonumber(r) or 1
            if setting[fmt].find_id_times ~= n then
                setting[fmt].find_id_times = n
            end
            height = height - 34
        end
        if fmt == 'slk' and setting[fmt].optimize_jass then
            if checkbox_tip(canvas, lang.ui.CONFUSION, lang.ui.CONFUSION_HINT, setting[fmt].confused) then
                setting[fmt].confused = not setting[fmt].confused
            end
            height = height - 34
            if setting[fmt].confused then
                local r = canvas:edit(setting[fmt].confusion, 100, function (c)
                    return (48 <= c and c <= 57) or (65 <= c and c <= 90) or (c == 95) or (97 <= c and c <= 122)
                end)
                if setting[fmt].confusion ~= r then
                    setting[fmt].confusion = r
                end
                height = height - 34
            end
        end
    end)

    canvas:layout_row_dynamic(height, 1)
    canvas:layout_row_dynamic(30, 1)
    canvas:text(backend.message, NK_TEXT_LEFT)
    canvas:layout_row_dynamic(10, 1)
    canvas:layout_row_dynamic(30, 1)
    if backend.lastword then
        if backend.lastword.type == 'failed' or backend.lastword.type == 'error' then
            window:set_theme(222, 55, 55)
        elseif backend.lastword.type == 'warning' then
            window:set_theme(255, 200, 55)
        end
        if canvas:button(backend.lastword.content) then
            if next(backend.report) then
                uitype = 'report'
            end
        end
        set_current_theme()
    else
        if backend.progress then
            canvas:progress(math.floor(backend.progress), 100)
        else
            canvas:text(current_tip or '', NK_TEXT_LEFT)
        end
    end
    canvas:layout_row_dynamic(10, 1)
    canvas:layout_row_dynamic(50, 1)
    if worker and not worker.exited then
        canvas:button(lang.ui.PROCESSING)
    else
        if canvas:button(lang.ui.START) then
            canvas:progress(0, 100)
            worker = backend:open(root / 'script' / 'backend' / 'init.lua', ('%s "%s"'):format(fmt, mappath:string()))
            backend.message = lang.ui.INIT
        end
    end
end

local function sortpairs(t)
    local sort = {}
    for k, v in pairs(t) do
        sort[#sort+1] = {k, v}
    end
    table.sort(sort, function (a, b)
        return a[1] < b[1]
    end)
    local n = 1
    return function()
        local v = sort[n]
        if not v then
            return
        end
        n = n + 1
        return v[1], v[2]
    end
end

local treecache = {n=2}
local function get_tree(name)
    if not treecache[name] then
        treecache.n = treecache.n + 1
        treecache[name] = treecache.n 
    end
    return treecache[name]
end

local function window_report(canvas)
    canvas:layout_row_dynamic(500, 1)
    canvas:group(lang.ui.REPORT, function()
        for type, report in sortpairs(backend.report) do
            if type ~= '' then
                local total = report[1][1]:match('TOTAL:(%d+)')
                local title = ('%s (%d)'):format(type:sub(2), total or #report)
                canvas:tree(title, get_tree(title), function()
                    for i, s in ipairs(report) do
                        if total and i == 1 then
                        else
                            canvas:text(s[1], NK_TEXT_LEFT, s[2])
                        end
                    end
                end)
            end
        end
        local report = backend.report['']
        if report then
            for _, s in ipairs(report) do
                canvas:text(s[1], NK_TEXT_LEFT, s[2])
            end
        end
    end)
    canvas:layout_row_dynamic(20, 1)
    canvas:layout_row_dynamic(30, 1)
    if canvas:button(lang.ui.BACK) then
        uitype = 'convert'
    end
end

local dot = 0
function window:draw(canvas)
    update_worker()
    if uitype == 'none' then
        window_none(canvas)
        return
    end
    if uitype == 'about' then
        window_about(canvas)
        return
    end
    if uitype == 'select' then
        window_select(canvas)
        return
    end
    if uitype == 'report' then
        window_report(canvas)
        return
    end
    if uitype == 'plugin' then
        window_plugin(canvas)
        return
    end
    window_convert(canvas)
end

window:run()
