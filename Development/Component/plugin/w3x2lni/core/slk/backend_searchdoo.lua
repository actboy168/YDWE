local unpack_buf
local unpack_pos

local function set_pos(...)
    unpack_pos = select(-1, ...)
    return ...
end

local function unpack(str)
    return set_pos(str:unpack(unpack_buf, unpack_pos))
end

return function(w2l)
    local buf = w2l:file_load('map', 'war3map.doo')
    if not buf then
        return
    end
    local destructable = {}
    local doodad = {}
    unpack_buf = buf
    unpack_pos = 1
    local _, version, _, n = unpack 'llll'
    if version < 8 then
        for i = 1, n do
            local id = unpack 'c4lfffffffbbl'
            destructable[id] = true
        end
    else
        for i = 1, n do
            local id = unpack 'c4lfffffffbbl'
            destructable[id] = true
            for j = 1, unpack 'l' do
                -- 因为会生成jass代码，所以这部分引用我们可以不分析
                for k = 1, unpack 'l' do
                    unpack 'll'
                end
            end
            unpack 'l'
        end
    end
    local _, n = unpack 'll'
    for i = 1, n do
        local id = unpack 'c4lll'
        doodad[id] = true
    end
    return destructable, doodad
end
