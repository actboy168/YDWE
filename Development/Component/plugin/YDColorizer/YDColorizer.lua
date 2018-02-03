local loader = {}

local function InitConfig()
    -- WESTRING_COD_TYPE_STRING=字符串
    -- WESTRING_UE_DLG_EDITVALUE=编辑单位值 - %s
    -- WESTRING_IE_DLG_EDITVALUE=编辑物品值 - %s
    -- WESTRING_AE_DLG_EDITVALUE=编辑技能值 - %s
    -- WESTRING_FE_DLG_EDITVALUE=编辑效果值 - %s
    -- WESTRING_BE_DLG_EDITVALUE=编辑可破坏物值 - %s
    -- WESTRING_DE_DLG_EDITVALUE=编辑装饰物值 - %s
    -- WESTRING_GE_DLG_EDITVALUE=编辑科技值 - %s
	local wes = require 'WEString'
	local str = ''
	if not wes.WESTRING_COD_TYPE_STRING then
		return
	end
    for _, v in pairs {
    	wes.WESTRING_UE_DLG_EDITVALUE,
    	wes.WESTRING_IE_DLG_EDITVALUE,
    	wes.WESTRING_AE_DLG_EDITVALUE,
    	wes.WESTRING_FE_DLG_EDITVALUE,
    	wes.WESTRING_BE_DLG_EDITVALUE,
    	wes.WESTRING_DE_DLG_EDITVALUE,
    	wes.WESTRING_GE_DLG_EDITVALUE,
    } do
    	str = str .. string.format(v, wes.WESTRING_COD_TYPE_STRING) .. '\n'
	end
	io.save(fs.ydwe_path() / 'plugin' / 'YDColorizer' / 'title.txt', str)
end

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
	InitConfig()
	
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
