local ffi = require "ffi"

local loader = {}
	
loader.load = function(path)
	local s, r = pcall(ffi.load, path:string())
			
	if not s then
		return false, r
	end
	loader.dll = r
	
	ffi.cdef[[   
	   uint32_t Start(uint32_t major, uint32_t minor);
	]]
	
	if 0 == loader.dll.Start(ydwe_version.minor, ydwe_version.revision) then
		return  false, 'failed in YDClient.dll!Start'
	end
	return true
end

loader.unload = function()	
end

return loader
