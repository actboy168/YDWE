require "mpq_util"

local _mt = {}

function _mt:__init()
	self.mpqs = {}
	self.paths = {}
end

function _mt:attach_archive(handle)
	table.insert(self.mpqs, handle)
end

function _mt:open_archive(file)
	local handle = mpq_util:stormlib(file, true)
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
	for _, path in ipairs(self.paths) do
		if fs.exists(path / filename) then
			return true
		end
	end
	for _, mpq in ipairs(self.mpqs) do
		if mpq:has(filename) then
			return true
		end
	end

	return false
end

function _mt:load(filename)
	for _, path in ipairs(self.paths) do
		if fs.exists(path / filename) then
			local r, e =  io.load(path / filename)
			if r then
				return r
			end
		end
	end
	for _, mpq in ipairs(self.mpqs) do
		local buf = mpq:load(filename)
		if buf then
			return buf
		end
	end

	return ''
end

function interface_stormlib()
	local obj = setmetatable({}, { __index = _mt })
	obj:__init()
	return obj
end
