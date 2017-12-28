local ll = require "ffi.loadlibrary"

local loader = {}
	
loader.load = function(path)
	if global_config["FeatureToggle"]["EnableShowInternalAttributeId"] ~= "1" then
		log.warn('failed: disable')
		return false
	end
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
