local compiler = require "compile.compiler"
local lnisaver = require 'w3x2lni.lnisaver'

local function make_option(config, war3ver)
	local option = {}
	-- 是否启用JassHelper
	option.enable_jasshelper = config.ScriptCompiler.EnableJassHelper ~= "0"
	-- 是否使用脚本模式编译
	option.enable_jasshelper_scriptonly = config.ScriptCompiler.EnableJassHelperScriptOnly == "1"
	-- 是否是调试模式
	option.enable_jasshelper_debug = config.ScriptCompiler.EnableJassHelperDebug == "1"
	-- 是否优化地图
	option.enable_jasshelper_optimization = config.ScriptCompiler.EnableJassHelperOptimization ~= "0"
	-- 是否启用cJass
	option.enable_cjass = config.ScriptCompiler.EnableCJass == "1"
	-- 是否启用YDTrigger
	option.enable_yd_trigger = plugin.loaders['YDTrigger'] ~= nil
	-- pjass的版本
	option.pjass = config.PJass.Option
	-- 代码注入选项
	-- cjass和脚本注入不能共存
	if option.enable_cjass then
		option.script_injection = false
	else
		option.script_injection = tonumber(config.ScriptInjection.Option)
	end
	-- 目标魔兽版本
	local save_type = tonumber(config.MapSave.Option)
	if save_type == 1 then
		-- 固定旧版本
		option.runtime_version = 20
	elseif save_type == 2 then
		-- 固定新版本
		option.runtime_version = 24
	else
		-- 按照当前版本或者双份
		option.runtime_version = war3ver
	end
	return option
end

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
	local result = compiler:compile(map_path, make_option(global_config, war3_version:is_new() and 24 or 20))
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
