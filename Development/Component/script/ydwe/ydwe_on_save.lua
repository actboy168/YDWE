local compiler = require "compile.compiler"
local lnisaver = require 'w3x2lni.lnisaver'

-- 本函数当保存地图时调用
-- event_data - 事件参数。table类型，包含了以下成员
--	map_path - 保存的地图路径，字符串类型
-- 返回值：0和正数表示保存成功，负数表示保存失败。如果成功编译，一般返回0
function event.EVENT_SAVE_MAP(event_data)
	log.debug("********************* on save start *********************")

	-- 刷新配置数据
	global_config_reload()

	-- 当前保存的地图路径
	local map_path = fs.path(event_data.map_path)
	log.trace("Saving " .. map_path:string())

	-- 编译地图
	local result = compiler:compile(map_path, global_config, war3_version:is_new() and 24 or 20)
	if result then
		-- 转换成Lni地图
		result = lnisaver(map_path)
	end

	log.debug("Result " .. tostring(result))
	log.debug("********************* on save end *********************")
	if result then return 0 else return -1 end
end

-- 本函数当保存地图时调用
-- event_data - 事件参数。table类型，包含了以下成员
--	map_path - 保存的地图路径，字符串类型
function event.EVENT_PRE_SAVE_MAP(event_data)
	log.debug("********************* on pre_save start *********************")
	local map_path = fs.path(event_data.map_path)
	log.trace("Saving " .. map_path:string())
	
	-- 如果地图文件带有只读属性，则先询问是否去掉只读属性
	-- 128 == 0200 S_IWUSR
	if fs.exists(map_path) and 0 == (map_path:permissions() & 128) then		
		if gui.yesno_message(nil, LNG.REMOVE_MAP_READONLY, map_path:string()) then
			log.trace("Remove the read-only attribute.")
			map_path:add_permissions(128)
		else
			log.trace("Don't remove the read-only attribute.")
		end
	end
		
	log.debug("********************* on pre_save end *********************")
	return 0
end

function event.EVENT_NEW_SAVE_MAP(event_data)
	log.debug("********************* on new save start *********************")

	-- 刷新配置数据
	global_config_reload()

    -- TODO
    
	log.debug("********************* on new save end *********************")
	if result then return 0 else return -1 end
end
