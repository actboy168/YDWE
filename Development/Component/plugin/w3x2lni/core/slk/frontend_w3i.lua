local select = select

local w2l

local function unpack_flag(flag)
    local tbl = {}
    for i = 0, 64 do
        local n = 1 << i
        if n > flag then
            break
        end
        if flag & n ~= 0 then
            tbl[#tbl+1] = i + 1
        end
    end
    return tbl
end

local function pack(...)
    local tbl = {...}
    tbl[#tbl] = nil
    return tbl
end

local mt = {}
mt.__index = mt

function mt:set_index(...)
    self.index = select(-1, ...)
    return ...
end

function mt:unpack(str)
    return self:set_index(str:unpack(self.content, self.index))
end

function mt:is_finish()
    return ('I1'):unpack(self.content, self.index) == 0xFF
end

function mt:add_head(chunk)
    chunk['地图'] = {
        ['文件版本']   = self:unpack 'l',
        ['地图版本']   = self:unpack 'l',
        ['编辑器版本'] = self:unpack 'l',
        ['地图名称']   = w2l:load_wts(self.wts, (self:unpack 'z')),
        ['作者名字']   = w2l:load_wts(self.wts, (self:unpack 'z')),
        ['地图描述']   = w2l:load_wts(self.wts, (self:unpack 'z')),
        ['推荐玩家']   = w2l:load_wts(self.wts, (self:unpack 'z')),
    }
    
    chunk['镜头'] = {
        ['镜头范围'] = pack(self:unpack 'ffffffff'),
        ['镜头范围扩充'] = pack(self:unpack 'llll'),
    }

    chunk['地形'] = {
        ['地图宽度'] = self:unpack 'l',
        ['地图长度'] = self:unpack 'l',
    }

    local flag = self:unpack 'L'
    chunk['选项'] = {
        ['关闭预览图']      = flag >> 0 & 1,
        ['自定义结盟优先权'] = flag >> 1 & 1,
        ['对战地图']        = flag >> 2 & 1,
        ['大型地图']        = flag >> 3 & 1,
        ['迷雾区域显示地形'] = flag >> 4 & 1,
        ['自定义玩家分组']   = flag >> 5 & 1,
        ['自定义队伍']      = flag >> 6 & 1,
        ['自定义科技树']    = flag >> 7 & 1,
        ['自定义技能']      = flag >> 8 & 1,
        ['自定义升级']      = flag >> 9 & 1,
        ['地图菜单标记']    = flag >> 10 & 1,
        ['地形悬崖显示水波'] = flag >> 11 & 1,
        ['地形起伏显示水波'] = flag >> 12 & 1,
        ['未知1']           = flag >> 13 & 1,
        ['未知2']           = flag >> 14 & 1,
        ['未知3']           = flag >> 15 & 1,
        ['未知4']           = flag >> 16 & 1,
        ['未知5']           = flag >> 17 & 1,
        ['未知6']           = flag >> 18 & 1,
        ['未知7']           = flag >> 19 & 1,
        ['未知8']           = flag >> 20 & 1,
        ['未知9']           = flag >> 21 & 1,
    }

    chunk['地形']['地形类型'] = self:unpack 'c1'

    chunk['载入图'] = {
        ['序号']   = self:unpack 'l',
        ['路径']   = w2l:load_wts(self.wts, (self:unpack 'z')),
        ['文本']   = w2l:load_wts(self.wts, (self:unpack 'z')),
        ['标题']   = w2l:load_wts(self.wts, (self:unpack 'z')),
        ['子标题'] = w2l:load_wts(self.wts, (self:unpack 'z')),
    }

    chunk['选项']['使用的游戏数据设置'] = self:unpack 'l'

    chunk['战役'] = {
        ['路径']   = w2l:load_wts(self.wts, (self:unpack 'z')),
        ['文本']   = w2l:load_wts(self.wts, (self:unpack 'z')),
        ['标题']   = w2l:load_wts(self.wts, (self:unpack 'z')),
        ['子标题'] = w2l:load_wts(self.wts, (self:unpack 'z')),
    }

    chunk['迷雾'] = {
        ['类型']    = self:unpack 'l',
        ['z轴起点'] = self:unpack 'f',
        ['z轴终点'] = self:unpack 'f',
        ['密度']    = self:unpack 'f',
        ['颜色']    = pack(self:unpack 'BBBB'),
    }
    
    chunk['环境'] = {
        ['天气']     = self:unpack 'c4',
        ['音效']     = w2l:load_wts(self.wts, (self:unpack 'z')),
        ['光照']     = self:unpack 'c1',
        ['水面颜色'] = pack(self:unpack 'BBBB'),
    }
end

function mt:add_player(chunk)
    chunk['玩家'] = {
        ['玩家数量'] = self:unpack 'l',
    }

    for i = 1, chunk['玩家']['玩家数量'] do
        chunk['玩家'..i] = {
            ['玩家']            = self:unpack 'l',
            ['类型']            = self:unpack 'l',
            ['种族']            = self:unpack 'l',
            ['修正出生点']       = self:unpack 'l',
            ['名字']            = w2l:load_wts(self.wts, (self:unpack 'z')),
            ['出生点']          = pack(self:unpack 'ff'),
            ['低结盟优先权标记'] = unpack_flag(self:unpack 'L'),
            ['高结盟优先权标记'] = unpack_flag(self:unpack 'L'),
        }
    end
end

function mt:unpack_player_flag(chunk)
    local flag = self:unpack 'L'
    local tbl = unpack_flag(flag)
    local exits = {}
    for i = 1, chunk['玩家']['玩家数量'] do
        local player = chunk['玩家'..i]
        local id = player['玩家'] + 1
        exits[id] = true
    end
    local result = {}
    for _, id in ipairs(tbl) do
        if exits[id] then
            result[#result+1] = id
        end
    end
    return result
end

function mt:add_force(chunk)
    chunk['队伍'] = {
        ['队伍数量'] = self:unpack 'l',
    }

    for i = 1, chunk['队伍']['队伍数量'] do
        local flag = self:unpack 'L'
        chunk['队伍'..i] = {
            ['结盟']            = flag >> 0 & 1,
            ['结盟胜利']        = flag >> 1 & 1,
            ['共享视野']        = flag >> 3 & 1,
            ['共享单位控制']     = flag >> 4 & 1,
            ['共享高级单位设置'] = flag >> 5 & 1,
            ['玩家列表']        = self:unpack_player_flag(chunk),
            ['队伍名称']        = w2l:load_wts(self.wts, (self:unpack 'z')),
        }
    end
end

function mt:add_upgrade(chunk)
    if self:is_finish() then
        return
    end
    local count = self:unpack 'l'
    for i = 1, count do
        chunk['升级'..i] = {
            ['玩家列表'] = unpack_flag(self:unpack 'L'),
            ['ID']      = self:unpack 'c4',
            ['等级']    = self:unpack 'l',
            ['可用性']  = self:unpack 'l',
        }
    end
end

function mt:add_tech(chunk)
    if self:is_finish() then
        return
    end
    local count = self:unpack 'l'
    for i = 1, count do
        chunk['科技'..i] = {
            ['玩家列表'] = unpack_flag(self:unpack 'L'),
            ['ID']      = self:unpack 'c4',
        }
    end
end

function mt:add_randomgroup(chunk)
    if self:is_finish() then
        return
    end
    local count = self:unpack 'l'
    for i = 1, count do
        chunk['随机组'..i] = {
            ['ID'] = self:unpack 'l',
            ['随机组名称'] = w2l:load_wts(self.wts, (self:unpack 'z')),
            ['设置'] = {},
        }
        local x = self:unpack 'l'
        chunk['随机组'..i]['位置类型'] = pack(self:unpack(('l'):rep(x)))

        local y = self:unpack 'l'
        for y = 1, y do
            chunk['随机组'..i]['设置'][y] = {
                ['几率'] = self:unpack 'l',
                ['ID']   = pack(self:unpack(('c4'):rep(x))),
            }
        end
    end
end

function mt:add_randomitem(chunk)
    if self:is_finish() then
        return
    end
    local count = self:unpack 'l'
    for i = 1, count do
        chunk['物品列表'..i] = {
            ['ID'] = self:unpack 'l',
            ['物品列表名称'] = w2l:load_wts(self.wts, (self:unpack 'z')),
            ['设置'] = {},
        }

        --设置
        local x = self:unpack 'l'
        for x = 1, x do
            chunk['物品列表'..i]['设置'][x] = {}
            local y = self:unpack 'l'
            for y = 1, y do
                chunk['物品列表'..i]['设置'][x][y] = {
                    ['几率'] = self:unpack 'l',
                    ['ID'] = self:unpack 'c4',
                }
            end
        end
    end
end

return function (w2l_, content, wts)
    if not content then
        return nil
    end
    w2l = w2l_
    
    local index = 1
    local tbl   = setmetatable({}, mt)
    local data  = {}

    tbl.content = content
    tbl.index   = index
    tbl.wts     = wts

    tbl:add_head(data)
    tbl:add_player(data)
    tbl:add_force(data)
    tbl:add_upgrade(data)
    tbl:add_tech(data)
    tbl:add_randomgroup(data)
    tbl:add_randomitem(data)
    
    return data
end
