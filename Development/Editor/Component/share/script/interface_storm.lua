require "ar_stormlib"

local _mt = {}

function _mt:__init()
	self.mpqs = {}
	self.paths = {}
end

function _mt:attach_archive(handle)
	table.insert(self.mpqs, handle)
end

function _mt:open_archive(file)
	local handle = ar.stormlib.open_archive(file, 0, ar.stormlib.MPQ_OPEN_READ_ONLY)
	if handle then
		self:attach_archive(handle)
	end
end

function _mt:open_path(file)
	if fs.exists(file) then
		table.insert(self.paths, file)
	else
		self:open_archive(file .. '.mpq')
	end
end

function _mt:has(filename)
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

function _mt:load(filename)
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

function interface_stormlib()
	local obj = setmetatable({}, { __index = _mt })
	obj:__init()
	return obj
end
