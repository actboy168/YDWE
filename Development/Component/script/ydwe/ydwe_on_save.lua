local compiler = require "compile.compiler"
local lnisaver = require 'w3x2lni.lnisaver'

-- 确定应当把地图保存为适合老版本（< 1.24）还是新版本（>= 1.24）的
-- 对于保存为双份，按照当前魔兽版本处理
-- save_type - 保存类型，0到3的取值，意义同设置程序，从上到下依次为0到3
-- runtime_version - 魔兽版本
-- 返回值: 数，如果保存的目标版本和当前版本一致，返回当前版本，否则视情况返回1.20或者1.24（和当前版本不同）
local function determine_map_version(save_type, runtime_version)
	-- 固定旧版本
	if save_type == 1 then
		return 20
	-- 固定新版本
	elseif save_type == 2 then
		return 24
	else
		-- 按照当前版本或者双份
		return runtime_version:is_new() and 24 or 20
	end
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

	-- 获取保存类型
	local save_type = tonumber(global_config["MapSave"]["Option"])

	-- 获取保存选项
	local save_option = {
		-- 是否启用JassHelper
		enable_jasshelper = (global_config["ScriptCompiler"]["EnableJassHelper"] ~= "0"),
		-- 是否使用脚本模式编译
		enable_jasshelper_scriptonly = (global_config["ScriptCompiler"]["EnableJassHelperScriptOnly"] == "1"),
		-- 是否是调试模式
		enable_jasshelper_debug = (global_config["ScriptCompiler"]["EnableJassHelperDebug"] == "1"),
		-- 是否优化地图
		enable_jasshelper_optimization = (global_config["ScriptCompiler"]["EnableJassHelperOptimization"] ~= "0"),
		-- 是否启用cJass
		enable_cjass = (global_config["ScriptCompiler"]["EnableCJass"] == "1"),
		-- 目标魔兽版本
		runtime_version = determine_map_version(save_type, war3_version),
		-- 代码注入选项
		script_injection = tonumber(global_config["ScriptInjection"]["Option"]),
		-- 是否启用YDTrigger
		enable_yd_trigger = plugin.loaders['YDTrigger'] ~= nil,
		-- pjass的版本
		pjass = global_config["PJass"]["Option"],
	}

	-- cjass和脚本注入不能共存
	if save_option.enable_cjass then
		save_option.script_injection = false
	end

	-- 编译地图
	local result = compiler:compile(map_path, save_option)
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
