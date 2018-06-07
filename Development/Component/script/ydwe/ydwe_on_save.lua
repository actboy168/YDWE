local compiler = require "compiler"
local objsaver = require 'w3x2lni.objsaver'
local lnisaver = require 'w3x2lni.lnisaver'

function event.EVENT_NEW_SAVE_MAP(event_data)
	log.debug("********************* on new save start *********************")

	-- 刷新配置数据
	global_config_reload()

	local map_path = fs.path(event_data.map_path)
	local temp_path = map_path:parent_path()
	local target_path = temp_path:parent_path() / map_path:filename()
	log.trace("Saving " .. target_path:string())

	-- 如果地图文件带有只读属性，则先询问是否去掉只读属性
	-- 128 == 0200 S_IWUSR
	if fs.exists(target_path) and 0 == (target_path:permissions() & 128) then		
		if gui.yesno_message(nil, LNG.REMOVE_MAP_READONLY, target_path:string()) then
			log.trace("Remove the read-only attribute.")
			target_path:add_permissions(128)
		else
            log.trace("Don't remove the read-only attribute.")
            log.debug("********************* on new save end *********************")
            return -1
		end
    end
    fs.remove(map_path)

	local result = objsaver(temp_path, map_path)
    if result then
        -- 编译地图
        result = compiler:compile(map_path, global_config, war3_version:is_new() and 24 or 20)
        if result then
            if map_path:filename():string() ~= '.w3x' then
                result = true
            else
                -- 转换成Lni地图
                result = lnisaver(map_path, target_path:parent_path())
            end
        end
    end

	log.debug("Result " .. tostring(result))
	log.debug("********************* on new save end *********************")
	if result then return 0 else return -1 end
end
