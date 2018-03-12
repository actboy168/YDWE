require 'filesystem'
require 'utility'
local messagebox = require 'ffi.messagebox'
local process = require 'process'
local nk = require 'nuklear'
local backend = require 'gui.backend'
local show_version = require 'gui.show_version'
local lni = require 'lni'
local currenttheme = {0, 173, 217}

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
local config = lni(io.load(root / 'config.ini'))
local fmt = nil

backend:init(root / 'bin' / 'w2l-worker.exe', root / 'script')

local config_content = [[
[root]
-- 使用谁的mpq(default, custom)
mpq = default
-- 使用的语言
lang = zh-CN

[lni]
-- 读取slk文件
read_slk = $lni.read_slk$
-- 限制搜索最优模板的次数,0表示无限
find_id_times = $lni.find_id_times$

[slk]
-- 简化(移除没有引用的对象)
remove_unuse_object = $slk.remove_unuse_object$
-- 优化脚本
optimize_jass = $slk.optimize_jass$
-- 压缩模型
mdx_squf = $slk.mdx_squf$
-- 删除只在WE中使用的文件
remove_we_only = $slk.remove_we_only$
-- 优化装饰物
slk_doodad = $slk.slk_doodad$
-- 限制搜索最优模板的次数,0表示无限
find_id_times = $slk.find_id_times$
-- 混淆脚本
confusion = $slk.confusion$

[obj]
-- 读取slk文件
read_slk = $obj.read_slk$
-- 限制搜索最优模板的次数,0表示无限
find_id_times = $obj.find_id_times$
]]

local function save_config()
    local buf = config_content:gsub('%$(.-)%$', function (str)
        local v = config
        for key in str:gmatch '[^%.]+' do
            v = v[key]
        end
        return tostring(v)
    end)
    io.save(root / 'config.ini', buf:gsub('\n', '\r\n'))
end

local window = nk.window('W3x2Lni', 400, 600)
window:set_theme(0, 173, 217)

local uitype = 'none'
local showmappath = false
local showcount = 0
local mapname = ''
local mappath = fs.path()

function window:dropfile(file)
    mappath = fs.path(file)
    mapname = mappath:filename():string()
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

local function button_about(canvas)
    canvas:layout_row_dynamic(20, 2)
    canvas:text('', NK_TEXT_RIGHT)
    window:set_style('button.color', 51, 55, 67)
    if canvas:button('版本: 1.9.1') then
        uitype = 'about'
    end
    set_current_theme()
end

local function window_about(canvas)
    canvas:layout_row_dynamic(20, 1)
    canvas:layout_space(30, 1)
    canvas:layout_space_push(-10, 0, 300, 30)
    canvas:button('作者')
    canvas:layout_row_dynamic(5, 1)
    canvas:layout_row_dynamic(20, 4)
    canvas:text('前端: ', NK_TEXT_RIGHT) canvas:text('actboy168', NK_TEXT_CENTERED) 
    canvas:layout_row_dynamic(20, 4)
    canvas:text('后端: ', NK_TEXT_RIGHT) canvas:text('最萌小汐', NK_TEXT_CENTERED)
    canvas:layout_row_dynamic(5, 1)
    canvas:layout_space(30, 1)
    canvas:layout_space_push(-10, 0, 300, 30)
    canvas:button('说明')
    show_version(window, canvas)
    reset_button_color()
    canvas:layout_row_dynamic(30, 1)
    if canvas:button('返回') then
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
    canvas:button('把地图拖进来')
    canvas:layout_row_dynamic(320, 1)
    button_about(canvas)
end

local function clean_convert_ui()
    backend:clean()
end

local function window_select(canvas)
    canvas:layout_row_dynamic(2, 1)
    canvas:layout_row_dynamic(100, 1)
    window:set_theme(0, 173, 217)
    if canvas:button('转为Lni') then
        uitype = 'convert'
        fmt = 'lni'
        window:set_title('W3x2Lni')
        config.mode = 'lni'
        save_config()
        clean_convert_ui()
        set_current_theme {0, 173, 217}
        return
    end
    window:set_theme(0, 173, 60)
    if canvas:button('转为Slk') then
        uitype = 'convert'
        fmt = 'slk'
        window:set_title('W3x2Slk')
        config.mode = 'slk'
        save_config()
        clean_convert_ui()
        set_current_theme {0, 173, 60}
        return
    end
    window:set_theme(217, 163, 60)
    if canvas:button('转为Obj') then
        uitype = 'convert'
        fmt = 'obj'
        window:set_title('W3x2Obj')
        config.mode = 'obj'
        save_config()
        clean_convert_ui()
        set_current_theme {217, 163, 60}
        return
    end
    canvas:layout_row_dynamic(212, 1)
    button_about(canvas)
end

local worker

local function update_worker()
    if worker then
        worker:update()
        if #worker.error > 0 then
            messagebox('错误', worker.error)
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
    if checkbox_tip(canvas, text, tip, config[fmt][data]) then
        config[fmt][data] = not config[fmt][data]
        save_config()
    end
end

local function window_convert(canvas)
    current_tip = nil
    local height = button_mapname(canvas, 290)
    canvas:layout_row_dynamic(10, 1)
    if fmt == 'lni' or fmt == 'obj' then
        height = height - 24
        canvas:layout_row_dynamic(30, 1)
        checkbox_simple(canvas, '读取slk文件', '外部导入的slk文件，如万能属性', 'read_slk')
    else
        height = height - 129
        canvas:layout_row_dynamic(30, 1)
        checkbox_simple(canvas, '简化', '删除没有使用的对象', 'remove_unuse_object')
        checkbox_simple(canvas, '优化脚本', '减小地图大小，加快运行时效率', 'optimize_jass')
        checkbox_simple(canvas, '压缩模型', '有损压缩', 'mdx_squf')
        checkbox_simple(canvas, '删除只在WE中使用的文件', '编辑器和本工具都将无法打开此地图', 'remove_we_only')
    end
    canvas:layout_row_dynamic(10, 1)
    canvas:tree('高级', 1, function()
        canvas:layout_row_dynamic(30, 1)
        if fmt == 'slk' then
            checkbox_simple(canvas, '优化装饰物', '地形变化不会卡顿，但之后需要重启魔兽', 'slk_doodad')
            height = height - 34
        end
        if checkbox_tip(canvas, '限制搜索最优模板的次数', '次数越多质量越好，但某些地图搜索会很慢', config[fmt].find_id_times ~= 0) then
            if config[fmt].find_id_times == 0 then
                config[fmt].find_id_times = 10
            else
                config[fmt].find_id_times = 0
            end
            save_config()
        end
        height = height - 34
        if config[fmt].find_id_times ~= 0 then
            local r = canvas:edit(tostring(config[fmt].find_id_times), 10, function (c)
                return 48 <= c and c <= 57
            end)
            local n = tonumber(r) or 1
            if config[fmt].find_id_times ~= n then
                config[fmt].find_id_times = n
                save_config()
            end
            height = height - 34
        end
        if fmt == 'slk' and config[fmt].optimize_jass then
            if checkbox_tip(canvas, '混淆脚本', '将变量名与函数名混淆为以下字符的组合', config[fmt].confusion ~= nil) then
                if config[fmt].confusion == nil then
                    config[fmt].confusion = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_'
                else
                    config[fmt].confusion = nil
                end
                save_config()
            end
            height = height - 34
            if config[fmt].confusion ~= nil then
                local r = canvas:edit(config[fmt].confusion, 100, function (c)
                    return (48 <= c and c <= 57) or (65 <= c and c <= 90) or (c == 95) or (97 <= c and c <= 122)
                end)
                if config[fmt].confusion ~= r then
                    config[fmt].confusion = r
                    save_config()
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
    if (worker and not worker.exited) or #backend.report == 0 then
        if backend.progress then
            canvas:progress(math.floor(backend.progress), 100)
        else
            canvas:text(current_tip or '', NK_TEXT_LEFT)
        end
    else
        if canvas:button('详情') then
            uitype = 'report'
        end
    end
    canvas:layout_row_dynamic(10, 1)
    canvas:layout_row_dynamic(50, 1)
    if worker and not worker.exited then
        canvas:button('正在处理...')
    else
        if canvas:button('开始') then
            canvas:progress(0, 100)
            worker = backend:open(root / 'script' / 'map.lua', ('"%s" -%s'):format(mappath:string(), config.mode))
            backend.message = '正在初始化...'
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
    canvas:group('详情', function()
        for type, report in sortpairs(backend.report) do
            if type ~= '' then
                type = type:sub(2)
                canvas:tree(type, get_tree(type), function()
                    for _, s in ipairs(report) do
                        canvas:text(s[1], NK_TEXT_LEFT, s[2])
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
    if canvas:button('返回') then
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
    window_convert(canvas)
end

window:run()
