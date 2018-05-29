local load = require 'map-builder.load'
local w2l

local mt = {}
mt.__index = mt

local function unify(name)
    return name:lower():gsub('/', '\\'):gsub('\\[\\]+', '\\')
end

function mt:set(name, buf)
    name = unify(name)
    self.cache[name] = buf
    if self.archive and self.type == 'output' then
        self.archive:set(name, buf)
    end
end

function mt:save(type, name, buf)
    if type == 'table' then
        self:set(w2l.info.lni_dir[name][1], buf)
    elseif type == 'trigger' then
        self:set('trigger/' .. name, buf)
    elseif type == 'scripts' then
        self:set('scripts/' .. name, buf)
    elseif type == 'w3x2lni' then
        self:set('w3x2lni/' .. name, buf)
    else
        if self.mode == 'lni' then
            self:set(type .. '/' .. name, buf)
        else
            self:set(name, buf)
        end
    end
end

function mt:get(name)
    name = unify(name)
    if self.cache[name] == false then
        return nil
    end
    if self.cache[name] ~= nil then
        return self.cache[name]
    end
    if self.archive and self.type == 'input' then
        return self.archive:get(name)
    end
end

function mt:load(type, name)
    if type == 'table' then
        for _, filename in ipairs(w2l.info.lni_dir[name]) do
            local buf = self:get(filename)
            if buf then
                return buf
            end
        end
    elseif type == 'trigger' then
        return self:get('trigger/' .. name) or self:get('war3map.wtg.lml/' .. name)
    elseif type == 'scripts' then
        return self:get('scripts/' .. name)
    elseif type == 'w3x2lni' then
        return self:get('w3x2lni/' .. name)
    else
        if self.mode == 'lni' then
            return self:get(type .. '/' .. name)
        else
            return self:get(name)
        end
    end
end

function mt:rm(name)
    name = unify(name)
    self.cache[name] = false
    if self.archive and self.type == 'output' then
        self.archive:remove(name)
    end
end

function mt:remove(type, name)
    if type == 'table' then
        for _, filename in ipairs(w2l.info.lni_dir[name]) do
            self:rm(filename)
        end
    elseif type == 'trigger' then
        self:rm('trigger/' .. name)
        self:rm('war3map.wtg.lml/' .. name)
    elseif type == 'scripts' then
        self:rm('scripts/' .. name)
    elseif type == 'w3x2lni' then
        self:rm('w3x2lni/' .. name)
    else
        if self.mode == 'lni' then
            self:rm(type .. '/' .. name)
        else
            self:rm(name)
        end
    end
end

function mt:pairs()
    if not self._loaded then
        self._loaded = load(w2l, self.archive)
    end
    local index
    local function next_one()
        local name, buf = next(self._loaded, index)
        if not name then
            return nil
        end
        index = name
        if self.cache[name] ~= nil then
            return next_one()
        end
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
    return next_one
end

local function load_file(path)
    local f = io.open(path, 'rb')
    if f then
        local buf = f:read 'a'
        f:close()
        return buf
    end
    return nil
end

return function (w2l_, archive, mode, type)
    w2l = w2l_
    local proxy = setmetatable({ archive = archive, mode = mode, type = type, cache = {} }, mt)
    if mode == 'lni' then
        local buf = load_file '.w3x'
        proxy:set('.w3x', buf)
    end
    return proxy
end
