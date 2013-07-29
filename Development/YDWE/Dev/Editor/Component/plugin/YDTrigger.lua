require "sys"

local loader = {}
	
loader.load = function(path)
	if global_config:get_integer("ScriptCompiler.EnableCJass", 0) ~= 0 then
		return false, 'enable cjass'
	end
	if global_config:get_integer("ThirdPartyPlugin.EnableYDTrigger", 1) ~= 1 then
		return false, 'diable'
	end
	
	-- YDTrigger MPQÂ·¾¶
	local yd_trigger_mpq_path = fs.ydwe_path() / "share" / "mpq" / "YDTrigger.mpq"
	if not fs.exists(yd_trigger_mpq_path) or not ar.storm.open_archive(yd_trigger_mpq_path, 15) then
		return false, 'cannot ' .. yd_trigger_mpq_path:filename():string()
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
