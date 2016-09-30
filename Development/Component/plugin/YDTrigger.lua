require "sys"

local loader = {}
	
loader.load = function(path)
	if global_config["ScriptCompiler"]["EnableCJass"] == "1" then
		log.warn('failed: enable cjass')
		return false
	end
	if global_config["ThirdPartyPlugin"]["EnableYDTrigger"] == "0" then
		log.warn('failed: disable')
		return false
	end
	
	-- YDTrigger MPQÂ·¾¶
	mpq_util:load_mpq("ydtrigger", 15)

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
