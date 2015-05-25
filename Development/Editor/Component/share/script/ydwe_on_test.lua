

-- 本函数在测试地图时使用
-- event_data - 事件参数，table，包含以下值
--	map_path - 保存的地图路径，字符串类型
--	application_name - 应用程序名，字符串对象
--	command_line - 命令行，字符串对象
-- 返回：非负表示成功，负数表示失败。如成功且无特殊情况原则上应该返回0
function event.EVENT_TEST_MAP(event_data)
	log.debug("********************* on test start *********************")

	-- 刷新配置数据
	global_config_reload()
	
	-- 获取当前测试的地图名
	local map_path = fs.path(event_data.map_path)
	log.debug("Testing " .. map_path:string())
	log.debug("Testing " .. event_data.command_line)

	-- 附加命令行
	local command_aux = ""

	-- 是否OpenGL方式？
	if global_config["MapTest"]["LaunchRenderingEngine"] == "OpenGL" then
		command_aux = command_aux .. " -opengl"
	end

	-- 是否窗口方式？
	if global_config["MapTest"]["LaunchWindowed"] ~= "0" then
		command_aux = command_aux .. " -window"
	end
	
	command_aux = command_aux .. ' -ydwe "' .. fs.ydwe_path():string() .. '"'
	
	local new_command_line
	local b, e = event_data.command_line:find(' %-loadfile ')
	if b then
		new_command_line = event_data.command_line:sub(1, b-1) .. ' -loadfile "'.. event_data.command_line:sub(e+1, -1) .. '"' .. command_aux
	else
		new_command_line = event_data.command_line .. command_aux
	end
	
	local result = false
	-- 启动魔兽开始测试...
	local war3_helper_dll = fs.ydwe_path() / "plugin" / "warcraft3" / "yd_loader.dll"
	result = sys.spawn_inject(event_data.application_name, new_command_line, nil, war3_helper_dll)

	log.debug("********************* on test end *********************")
	if result then return 0 else return -1 end
end
