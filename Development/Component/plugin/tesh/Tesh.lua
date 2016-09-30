require "sys"

local loader = {}
	
loader.load = function(path)
	if global_config["ThirdPartyPlugin"]["EnableTesh"] == "0" then
		log.warn('failed: disable')
		return false
	end
	loader.dll = sys.load_library(path)
	return loader.dll ~= nil
end

loader.unload = function()
	if loader.dll then
		sys.unload_library(loader.dll)
		loader.dll = nil
	end
end

return loader
