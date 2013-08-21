local loader = {}
	
loader.load = function(path)
	if global_config:get_integer("ThirdPartyPlugin.EnableDotNetSupport", 1) ~= 1 then
		return false, 'disable'
	end
	
	require "dotnet"
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
