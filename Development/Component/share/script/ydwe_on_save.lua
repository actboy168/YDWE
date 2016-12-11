require "compile.inject_code"
require "compile.wave"
require "compile.template"
require "compile.cjass"
require "compile.jasshelper"
require "compile.native"
require "mpq_util"

-- 确定应当把地图保存为适合老版本（< 1.24）还是新版本（>= 1.24）的
-- 对于保存为双份，按照当前魔兽版本处理
-- save_type - 保存类型，0到3的取值，意义同设置程序，从上到下依次为0到3
-- runtime_version - 魔兽版本
-- 返回值: 数，如果保存的目标版本和当前版本一致，返回当前版本，否则视情况返回1.20或者1.24（和当前版本不同）
local function determine_map_version(save_type, runtime_version)
	-- 固定旧版本
	if save_type == 1 then
		return runtime_version:is_new() and runtime_version:old() or runtime_version
	-- 固定新版本
	elseif save_type == 2 then
		return runtime_version:is_new() and runtime_version or runtime_version:new()
	else
		-- 按照当前版本或者双份
		return runtime_version
	end
end

-- 编译地图
-- map_path - 地图路径，string或者fs.path
-- option - 编译选项，table类型，支持的选项：
--	enable_preprocessor - 启用wave预处理器，true/false
-- 	enable_jasshelper - 启用JassHelper，true/false
--	enable_jasshelper_debug - 启用JassHelper的Debug，true/false
--	enable_jasshelper_optimization - 启用优化，true/false
--	enable_cjass - 启用cJass编译器，true/false
--	script_injection - 代码注入选项，整数，0表示智能注入，1表示不注入
--	runtime_version - 魔兽版本
-- 返回：true表示成功，false表示失败
local function compile_map(map_path, option)
	-- 统一处理fs.path
	if type(map_path) == "string" then map_path = fs.path(map_path) end
	-- 结果
	local result = nil

	log.trace("Save version " .. tostring(option.runtime_version))

	local compile_t = {
		['option'] = option,
		['map_path'] = map_path,
	}
	
	-- 如果JassHelper开启，执行正常编译
	if option.enable_jasshelper then
		result = mpq_util:update_file(map_path, "war3map.j",
			-- 解压缩地图脚本，处理然后写回
			function (map_handle, in_script_path)
				-- 开始处理
				log.trace("Processing " .. in_script_path:filename():string())

				compile_t.input = in_script_path
				compile_t.output = nil
				compile_t.map_handle = map_handle
				compile_t.inject_file = function (file_path, path_in_archive)
					log.trace("[stormlib]import file", path_in_archive)
					return map_handle:add_file(path_in_archive, file_path)
				end
				
				-- 未启用用cJass
				if not option.enable_cjass then
					-- 根据注入选项进行处理（由于Lua的closure，此处可以访问“父”函数的局部变量）
					if option.script_injection == 0 then
						if not inject_code:compile(compile_t) then
							return nil
						end
						compile_t.input = compile_t.output
					end

					-- Wave预处理
					if not wave:compile(compile_t) then
						return nil
					end
					compile_t.input = compile_t.output
				end

				if not template:compile(compile_t) then
					return nil
				end
				
				return compile_t.output
			end
		)
		
		-- 开始调用编译工具编译
		if result then
			-- 调用cJass
			if option.enable_cjass then
				result = cjass:compile(map_path, option)
			end

			-- 调用jasshelepr
			if result then
				result = jasshelper:compile(map_path, option)
			end
		end

		native:compile(compile_t)
	else
		-- 只做语法检查
		result = jasshelper:compile(map_path, option)
	end

	return result
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

	-- 如果保存双份，需要预先拷贝
	local map_path_aux = nil
	if save_type == 3 then
		-- 决定第二份的路径
		if save_option.runtime_version:is_new() then
			map_path_aux = fs.aux_filename(map_path, "hashtable")
		else
			map_path_aux = fs.aux_filename(map_path, "returnbug")
		end

		log.trace("Making copy of the original map.")

		-- true代表覆盖现有文件。false不会覆盖
		pcall(fs.copy_file, map_path, map_path_aux, true)
	end

	-- 编译地图
	local result = compile_map(map_path, save_option)

	-- 如果有第二份，此时处理第二份
	if map_path_aux then
		log.trace("Processing second map")
		save_option.runtime_version = save_option.runtime_version:is_new() and save_option.runtime_version:old() or save_option.runtime_version:new()
		compile_map(map_path_aux, save_option)
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
		if gui.yesno_message(nil, _("Whether to remove the read-only attribute from \"%s\"?"), map_path:string()) then
			log.trace("Remove the read-only attribute.")
			map_path:add_permissions(128)
		else
			log.trace("Don't the remove read-only attribute.")
		end
	end
		
	log.debug("********************* on pre_save end *********************")
	return 0
end
