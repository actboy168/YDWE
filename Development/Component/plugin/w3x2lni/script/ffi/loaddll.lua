local ffi = require 'ffi'
local uni = require 'ffi.unicode'
local getmodule = require 'ffi.getmodule'

ffi.cdef[[
    int LoadLibraryW(const wchar_t* libname);
]]

local function getfullpath(name)
    local path, e = package.searchpath(name, package.cpath)
    if not path then
        local path = os.getenv 'PATH'
	    if path then
    		for w in path:gmatch '[^;]+' do
	    		local r = w .. '\\' .. name .. '.dll'
    			if io.open(r, 'rb') then
	    			return r
    			end
    		end
		end
        return error(e)
    end
    return path
end

return function(name)
    local h = getmodule(name)
    if h ~= 0 then
        return h
    end
    local wpath = uni.u2w(getfullpath(name))
	return ffi.C.LoadLibraryW(wpath)
end
