require 'filesystem'
local info = require 'core.info'
local root = fs.current_path()

local mt = {}
mt.__index = mt

function mt:save(type, name, buf)
    if type == 'table' then
        self.archive:set(info.lni_dir[name][1], buf)
    elseif type == 'trigger' then
        self.archive:set('trigger/' .. name, buf)
    elseif type == 'scripts' then
        self.archive:set('scripts/' .. name, buf)
    elseif type == 'w3x2lni' then
        self.archive:set('w3x2lni/' .. name, buf)
    else
        if self.mode == 'lni' then
            self.archive:set(type .. '/' .. name, buf)
        else
            self.archive:set(name, buf)
        end
    end
end

function mt:load(type, name)
    if type == 'table' then
        for _, filename in ipairs(info.lni_dir[name]) do
            local buf = self.archive:get(filename)
            if buf then
                return buf
            end
        end
    elseif type == 'trigger' then
        return self.archive:get('trigger/' .. name) or self.archive:get('war3map.wtg.lml/' .. name)
    elseif type == 'scripts' then
        return self.archive:get('scripts/' .. name)
    elseif type == 'w3x2lni' then
        return self.archive:get('w3x2lni/' .. name)
    else
        if self.mode == 'lni' then
            return self.archive:get(type .. '/' .. name)
        else
            return self.archive:get(name)
        end
    end
end

function mt:remove(type, name)
    if type == 'table' then
        for _, filename in ipairs(info.lni_dir[name]) do
            self.archive:remove(filename)
        end
    elseif type == 'trigger' then
        self.archive:remove('trigger/' .. name)
        self.archive:remove('war3map.wtg.lml/' .. name)
    elseif type == 'scripts' then
        self.archive:remove('scripts/' .. name)
    elseif type == 'w3x2lni' then
        self.archive:remove('w3x2lni/' .. name)
    else
        if self.mode == 'lni' then
            self.archive:remove(type .. '/' .. name)
        else
            self.archive:remove(name)
        end
    end
end

function mt:pairs()
    local next, tbl, index = self.archive:search_files()
    return function ()
        local name, buf = next(tbl, index)
        if not name then
            return nil
        end
        index = name
        local type
        local dir = name:match '^[^/\\]+' :lower()
        local ext = name:match '[^%.]+$'
        if ext == 'mdx' or ext == 'mdl' or ext == 'blp' or ext == 'tga' then
            type = 'resource'
        elseif ext == 'mp3' or ext == 'wav' then
            type = 'sound'
        elseif name == 'scripts\\war3map.j' then
            type = 'map'
        elseif dir == 'scripts' then
            type = 'scripts'
        elseif dir == 'w3x2lni' then
            type = 'w3x2lni'
        else
            type = 'map'
        end
        if self.mode == 'lni' or type == 'scripts' or type == 'w3x2lni' then
            if dir == type then
                name = name:sub(#type + 2)
            end
        end
        return type, name, buf
    end
end

return function (archive, mode)
    if mode == 'lni' then
        local buf = io.load(root / 'map-builder' / '.w3x')
        archive:set('.w3x', buf)
    end
    return setmetatable({ archive = archive, mode = mode }, mt)
end
