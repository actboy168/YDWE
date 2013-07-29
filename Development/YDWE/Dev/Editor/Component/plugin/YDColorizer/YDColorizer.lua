--[[
require "dotnet"

local loader = {}
	
loader.load = function(path)
	if global_config:get_integer("ThirdPartyPlugin.EnableDotNetSupport", 1) ~= 1 then
		return false, 'disable'
	end
	if not dotnet.initialize() then
		return false, 'not support .net.'
	end

	loader.dll = dotnet.load(path)
	if not loader.dll then
		return false
	end
	
	local WEInit = loader.dll:GetType("WEInit")
	if not WEInit then
		return false, 'cannot found WEInit.'
	end
	
	local s, r = dotnet.call_func(WEInit)
			
	if not s then
		return false, r
	end
	return true
end

loader.unload = function()	
end

return loader
--]]
require "dotnet"

local loader = {}
	
loader.load = function(path)
	if global_config:get_integer("ThirdPartyPlugin.EnableDotNetSupport", 1) ~= 1 then
		return false, 'disable'
	end
	
	local obj = dotnet:load(path, 'WEInit')
	if not obj then
		return false, 'load failed'
	end
	
	if not obj:call('begin') then
		return false, 'call failed'
	end
	
	return true
end

loader.unload = function()	
end

return loader
