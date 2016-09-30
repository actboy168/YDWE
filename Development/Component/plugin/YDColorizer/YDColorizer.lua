local loader = {}
	
loader.load = function(path)
	if global_config["ThirdPartyPlugin"]["EnableDotNetSupport"] ~= "1" then
		log.warn('failed: disable')
		return false
	end
	
	require "dotnet"
	if not dotnet.initialized then
		log.error('failed: not support')
		return false
	end
	
	local obj = dotnet:load(path, 'WEInit')
	if not obj then
		log.error('failed: load failed')
		return false
	end
	if obj:error_code() ~= 0 then
		log.error(string.format('failed: load failed(%08X)',  obj:error_code()))
		return false
	end
	
	local result, msg = obj:call('begin')
	if result == nil then
		if msg then
			log.error('failed: call failed(' .. msg .. ')')
			return false
		else
			log.error('failed: call failed(unknown)')
			return false
		end
	elseif result ~= 0 then
		log.error(string.format('failed: call failed(%08X)',  result))
		return false
	end
	
	return true
end

loader.unload = function()	
end

return loader
