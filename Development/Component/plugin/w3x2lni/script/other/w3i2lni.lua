local table_concat = table.concat
local type = type
local math_type = math.type

local mt = {}
mt.__index = mt

function mt:add(format, ...)
    self.lines[#self.lines+1] = format:format(...)
end

local function format_value(value)
    local tp = type(value)
    if tp == 'string' then
        if value:match '[\n\r]' then
            return ('[=[\r\n%s]=]'):format(value)
        else
            return ('%q'):format(value)
        end
    elseif tp == 'number' then
        if math_type(value) == 'integer' then
            return value
        else
            return ('%.4f'):format(value)
        end
    end
end

local function format_table(tbl)
    local new_tbl = {}
    for i = 1, #tbl do
        new_tbl[i] = format_value(tbl[i])
    end
    return new_tbl
end

function mt:title(name, data)
    if not data[name] then
        return nil
    end
    if self._title then
        self:add ''
    end
    self._title = data[name]
    self:add(('[%s]'):format(name))
    return data[name]
end

function mt:value(name)
    local value = self._title[name]
    if type(value) == 'table' then
        local strs = format_table(value)
        if type(strs[1]) == 'string' and #strs > 6 then
            self:add('%s = {\r\n%s\r\n}', name, table_concat(strs, ',\r\n'))
        else
            self:add('%s = {%s}', name, table_concat(strs, ', '))
        end
    else
        self:add('%s = %s', name, format_value(value))
    end
end

function mt:add_head(data)
    self:title('地图', data)
    self:value '文件版本'
    self:value '地图版本'
    self:value '编辑器版本'
    self:value '地图名称'
    self:value '作者名字'
    self:value '地图描述'
    self:value '推荐玩家'

    self:title('镜头', data)
    self:value '镜头范围'
    self:value '镜头范围扩充'

    self:title('地形', data)
    self:value '地图宽度'
    self:value '地图长度'
    self:value '地形类型'
    
    self:title('选项', data)
    self:value '使用的游戏数据设置'
    self:value '关闭预览图'
    self:value '自定义结盟优先权'
    self:value '对战地图'
    self:value '大型地图'
    self:value '迷雾区域显示地形'
    self:value '自定义玩家分组'
    self:value '自定义队伍'
    self:value '自定义科技树'
    self:value '自定义技能'
    self:value '自定义升级'
    self:value '地图菜单标记'
    self:value '地形悬崖显示水波'
    self:value '地形起伏显示水波'
    self:value '未知1'
    self:value '未知2'
    self:value '未知3'
    self:value '未知4'
    self:value '未知5'
    self:value '未知6'
    self:value '未知7'
    self:value '未知8'
    self:value '未知9'
    
    self:title('载入图', data)
    self:value '序号'
    self:value '路径'
    self:value '文本'
    self:value '标题'
    self:value '子标题'

    self:title('战役', data)
    self:value '路径'
    self:value '文本'
    self:value '标题'
    self:value '子标题'

    self:title('迷雾', data)
    self:value '类型'
    self:value 'z轴起点'
    self:value 'z轴终点'
    self:value '密度'
    self:value '颜色'

    self:title('环境', data)
    self:value '天气'
    self:value '音效'
    self:value '光照'
    self:value '水面颜色'

    return data
end

function mt:add_player(data)
    self:title('玩家', data)
    self:value '玩家数量'

    for i = 1, data['玩家']['玩家数量'] do
        self:title('玩家'..i, data)
        self:value '玩家'
        self:value '类型'
        self:value '种族'
        self:value '修正出生点'
        self:value '名字'
        self:value '出生点'
        self:value '低结盟优先权标记'
        self:value '高结盟优先权标记'
    end
end

function mt:add_force(data)
    self:title('队伍', data)
    self:value '队伍数量'

    for i = 1, data['队伍']['队伍数量'] do
        self:title('队伍'..i, data)
        self:value '结盟'
        self:value '结盟胜利'
        self:value '共享视野'
        self:value '共享单位控制'
        self:value '共享高级单位设置'
        self:value '玩家列表'
        self:value '队伍名称'
    end
end

function mt:add_upgrade(data)
    local i = 1
    while self:title('升级'..i, data) do
        self:value '玩家列表'
        self:value 'ID'
        self:value '等级'
        self:value '可用性'
        i = i + 1
    end
end

function mt:add_tech(data)
    local i = 1
    while self:title('科技'..i, data) do
        self:value '玩家列表'
        self:value 'ID'
        i = i + 1
    end
end

function mt:add_randomgroup(data)
    local i = 1
    while self:title('随机组'..i, data) do
        self:value '随机组名称'
        self:value '位置类型'

        self:add('设置 = {')
        for i, set in ipairs(data['随机组'..i]['设置']) do
            self:add('%d = {', i)
            self:add('几率 = %d,', set['几率'])
            self:add('ID = {%s},', table_concat(format_table(set['ID']), ', '))
            self:add('},')
        end
        self:add('}')
        i = i + 1
    end
end

function mt:add_randomitem(data)
    local i = 1
    while self:title('物品列表'..i, data) do
        self:value '物品列表名称'

        self:add('设置 = {')
        for i, set in ipairs(data['物品列表'..i]['设置']) do
            self:add('%d = {', i)
            for _, item in ipairs(set) do
                self:add('{几率 = %d, ID = %q},', item['几率'], item['ID'])
            end
            self:add('},')
        end
        self:add('}')
        i = i + 1
    end
end

return function (w2l, data, wts)
    local tbl = setmetatable({}, mt)
    tbl.lines = {}
    tbl.self = w2l

    tbl:add_head(data)
    tbl:add_player(data)
    tbl:add_force(data)
    tbl:add_upgrade(data)
    tbl:add_tech(data)
    tbl:add_randomgroup(data)
    tbl:add_randomitem(data)

    return table_concat(tbl.lines, '\r\n')
end
