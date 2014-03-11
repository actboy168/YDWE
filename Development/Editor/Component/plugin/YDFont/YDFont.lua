local ffi = require "ffi"

local loader = {}
	
loader.load = function(path)
	if global_config:get_integer("Font.Enable", 0) == 0 then
		log.warn('failed: enable font')
		return false
	end
	
	local s, r = pcall(ffi.load, path:string())
			
	if not s then
		log.error('failed: ' .. r)
		return false
	end
	loader.dll = r

	ffi.cdef[[
		bool SetFontByName(const char* name, size_t size);
	]]

	if not loader.dll.SetFontByName(global_config:get_string("Font.Name", "system"), global_config:get_integer("Font.Size", 12)) then
		log.error('failed: in YDFont.dll!SetFontByName')
		return  false
	end
	return true
end

loader.unload = function()
end

return loader
