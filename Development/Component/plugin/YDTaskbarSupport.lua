require "localization"
local ffi = require "ffi"
local uni = require "ffi.unicode"

local loader = {}
	
loader.load = function(path)
	local s, r = pcall(ffi.load, path:string())
	if not s then
		log.error('failed: ' .. r)
		return false
	end
	loader.dll = r	
	ffi.cdef[[   
	   void Initialize();
	]]
	loader.dll.Initialize()
	return true
end

loader.unload = function()	
end

return loader
