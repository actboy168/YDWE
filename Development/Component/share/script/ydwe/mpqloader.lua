local event = require 'ev'

local function new_path()
    local mt = {}
    local paths = {'\\'}
    local mpqs = {}
    local function update()
        paths = {'\\'}
        for i = #mpqs, 1, -1 do
            local path = mpqs[i]
            local max = #paths
            table.insert(paths, '\\' .. path .. '\\')
            for i = 2, max do
                table.insert(paths, '\\' .. path .. paths[i])
            end
        end
    end
    function mt:open(path)
        table.insert(mpqs, path)
        update()
    end
    function mt:close(path)
        for i, mpq in ipairs(mpqs) do
            if mpq == path then
                table.remove(mpqs, i)
                update()
                return
            end
        end
    end
    function mt:each_path(callback)
        for i = #paths, 1, -1 do
            local res = callback(paths[i])
            if res then
                return res
            end
        end
    end
    function mt:first_path()
        return paths[#paths]
    end
    return mt
end

local mt = {}

function mt:init()
    local lang = (require "i18n").get_language()
    self.mpq_path = new_path()
    self.mpq_path:open(lang)
    event.on('virtual_mpq: open path', function(name)
        log.info('OpenPathAsArchive', name)
        self.mpq_path:open(name)
    end)
    event.on('virtual_mpq: close path', function(name)
        log.info('ClosePathAsArchive', name)
        self.mpq_path:close(name)
    end)
end

function mt:load(dir, filename)
	return self.mpq_path:each_path(function(path)
		return io.load(dir / path / filename)
	end)
end

function mt:save(dir, filename, buf)
    io.save(dir / self.mpq_path:first_path() / filename, buf)
end

return mt
