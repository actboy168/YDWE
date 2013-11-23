require "mapanalyzer"
require "ar_stormlib"

class 'interface_stormlib' (mapanalyzer.interface_storm)

function interface_stormlib:__init()
	mapanalyzer.interface_storm.__init(self)
	self.mpqs = {}
	self.paths = {}
end

function interface_stormlib:attach_archive(handle)
	table.insert(self.mpqs, handle)
end

function interface_stormlib:open_archive(file)
	local handle = ar.stormlib.open_archive(file, 0, ar.stormlib.MPQ_OPEN_READ_ONLY)
	if handle then
		self:attach_archive(handle)
	end
end

function interface_stormlib:open_path(file)
	if fs.exists(file) then
		table.insert(self.paths, file)
	else
		self:open_archive(file .. '.mpq')
	end
end

function interface_stormlib:has(filename)
	for _, v in ipairs(self.paths) do
		if fs.exists(v / filename) then
			return true
		end
	end
	for _, v in ipairs(self.mpqs) do
		if ar.stormlib.has_file(v, filename) then
			return true
		end
	end

	return false
end

function interface_stormlib:load(filename)
	for _, v in ipairs(self.paths) do
		if fs.exists(v / filename) then
			local r, e =  io.load(v / filename)
			if r then
				return r
			end
		end
	end
	for _, v in ipairs(self.mpqs) do
		local r, buffer = ar.stormlib.load_file(v, filename)
		if r then
			return buffer
		end
	end

	return ''
end

class 'interface_stormdll' (mapanalyzer.interface_storm)

function interface_stormdll:__init()
	mapanalyzer.interface_storm.__init(self)
end

function interface_stormdll:has(filename)
	return ar.storm.has_file(filename)
end

function interface_stormdll:load(filename)
	local r, buffer = ar.storm.load_file(filename)
	return r and buffer or ''
end
