require "sys"

local _version__index = {
}

local _version__lt = function (a,b)
	if a.major ~= b.major then
		return a.major < b.major
	end
	if a.minor ~= b.minor then
		return a.minor < b.minor
	end
	if a.revision ~= b.revision then
		return a.revision <b.revision
	end
	if a.build ~= b.build then
		return a.build < b.build
	end
	return false
end

local _version__eq = function (a,b)
	return ((a.major == b.major) 
		and (a.minor == b.minor)
		and (a.revision < b.revision)
		and (a.build < b.build))
end

local _version__le = function (a,b)
	return ((a < b) or (a == b))
end

local _version__tostring = function (self)
	return string.format('%d.%d.%d.%d', self.major, self.minor, self.revision, self.build)
end

local _version__mt = {
	__index    = _version__index,
	__lt       = _version__lt,
	__le       = _version__le,
	__eq       = _version__eq,
	__tostring = _version__tostring,
}

local _version_initialize = function (c)
	if c.file then
		if type(c.file) == 'string' then
			return sys.get_module_version_info(fs.path(c.file))
		else
			return sys.get_module_version_info(c.file)
		end
	end
	
	local obj = {}
	obj.major    = c.major or 0
	obj.minor    = c.minor or 0
	obj.revision = c.revision or 0
	obj.build    = c.build or 0
	return obj
end

function sys.version (c)
	return setmetatable(_version_initialize(c), _version__mt)
end

local _war3_version_124 = nil
local _war3_version_120 = nil

local _war3_version__index = {
	is_new = function (self)
		return self >= self:new()
	end,
	new = function (self)
		if not _war3_version_124 then
			_war3_version_124 = sys.war3_version { major = 1, minor = 24}
		end
		return _war3_version_124
	end,
	old = function (self)
		if not _war3_version_120 then
			_war3_version_120 = sys.war3_version { major = 1, minor = 20}
		end
		return _war3_version_120
	end,
}

local _war3_version__mt = {
	__index    = _war3_version__index,
	__lt       = _version__lt,
	__le       = _version__le,
	__eq       = _version__eq,
	__tostring = _version__tostring,
}

function sys.war3_version (c)
	return setmetatable(_version_initialize(c), _war3_version__mt)
end
