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
	if obj:error_code() ~= 0 then
		return false, string.format('load failed(%08X)',  obj:error_code())
	end
	
	local result, msg = obj:call('begin')
	if result == nil then
		if msg then
			return false, 'call failed(' .. msg .. ')'
		else
			return false, 'call failed(unknown)'
		end
	elseif result ~= 0 then
		return false, string.format('call failed(%08X)',  result)
	end
	
	return true
end

loader.unload = function()	
end

return loader
