require "sys"

local loader = {}
	
loader.load = function(path)
	if global_config:get_integer("ThirdPartyPlugin.EnableTesh", 1) ~= 1 then
		return false, 'disable'
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
