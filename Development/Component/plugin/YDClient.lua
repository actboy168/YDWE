require "localization"
local ffi = require "ffi"

local loader = {}
	
loader.load = function(path)
	local s, r = pcall(ffi.load, __(path:string()))
			
	if not s then
		log.error('failed: ' .. r)
		return false
	end
	loader.dll = r
	
	ffi.cdef[[   
	   uint32_t Start(uint32_t major, uint32_t minor);
	]]
	
	if 0 == loader.dll.Start(ydwe_version.minor, ydwe_version.revision) then
		log.error('failed: in YDClient.dll!Start')
		return  false
	end
	return true
end

loader.unload = function()	
end

return loader
