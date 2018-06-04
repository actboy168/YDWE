local ll = require "ffi.loadlibrary"

local loader = {}
	
loader.load = function(path)
	loader.dll = ll.load_library(path)
	return loader.dll ~= nil
end

loader.unload = function()
	if loader.dll then
		ll.unload_library(loader.dll)
		loader.dll = nil
	end
end

return loader
