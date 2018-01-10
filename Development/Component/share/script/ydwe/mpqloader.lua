local event = require 'ev'
local mpq_path = require 'w3x2lni.mpq_path'

local mt = {}

function mt:init()
    local lang = (require "i18n").get_language()
    self.mpq_path = mpq_path()
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

function mt:create_directories(dir)
    fs.create_directories(dir / self.mpq_path:first_path())
end

return mt
