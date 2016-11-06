
local storm = ar.storm

-- 主窗口句柄
local main_window_handle = nil

-- 启动配置对话框
local function launch_config()
	local config_program = fs.ydwe_path() / "bin" / "YDWEConfig.exe"
	local command_line = string.format('"%s"', config_program:string())
	sys.spawn(command_line, fs.ydwe_path())
end

-- 启动魔兽
local function launch_warcraft3()
	local config_program = fs.ydwe_path() / "bin" / "YDWEConfig.exe"
	local command_line = string.format('"%s" -launchwar3', config_program:string())
	sys.spawn(command_line, fs.ydwe_path())
end

-- 显示最近一次编译错误
local function show_last_error()
	local jasshelper_path = fs.ydwe_path() / "plugin" / "jasshelper" / "jasshelper.exe"
	local command_line = string.format('"%s" --showerrors', jasshelper_path:string())
	sys.spawn(command_line, fs.ydwe_path())
end

-- 显示JassHelper版本
local function show_jasshelper_version()
	local jasshelper_path = fs.ydwe_path() / "plugin" / "jasshelper" / "jasshelper.exe"
	local command_line = string.format('"%s" --about', jasshelper_path:string())
	sys.spawn(command_line, fs.ydwe_path())
end

-- 显示cJass版本
local function show_cjass_version()
	local cjass_path = fs.ydwe_path() / "plugin" / "AdicHelper" / "AdicHelper.exe"
	local command_line = cjass_path:string()
	sys.spawn(command_line, fs.ydwe_path())
end

-- 打开官网
local function open_offical_site()
	os.execute('explorer "http://www.ydwe.net"')
end

-- 导出WE文件
local function file_export()
	-- 打开对话框让用户输入
	local ok, file_path_string = gui.prompt_for_input(
		main_window_handle, 																		-- 父窗口句柄
		_("Export file"),														-- 标题栏
		_("Please enter the file path to export. Currently you can export only one file each time"),																				-- 提示语句
		"",																		-- 文本编辑区初始文字
		_("OK"),																-- “确定”按钮文本
		_("Cancel")																-- “取消"按钮文本
	)

	if ok then
		if storm.has_file(file_path_string) then
			-- 弹出选择保存文件对话框
			local ok, file = gui.choose_save_file(
				main_window_handle, 														-- HWND
				"*.*\0*.*\0", 											-- Filter
				fs.path(file_path_string):filename():string(),					-- Initial file name
				nil, 													-- Initial Dir
				_("Save file")											-- Title
			)
			if ok then
				-- 解压文件
				storm.extract_file(fs.path(file), file_path_string)
			end
		else
			gui.error_message(nil, _("The file you have entered does not exist."))
		end
	end
end

-- Lua测试
local function lua_test()
	-- 打开对话框让用户输入
	local ok, script_string = gui.prompt_for_input(
		main_window_handle, 													-- 父窗口句柄
		_("Lua Test"),															-- 标题栏
		_("Please enter the lua script to execute"),							-- 提示语句
		"",																		-- 文本编辑区初始文字
		_("OK"),																-- “确定”按钮文本
		_("Cancel")																-- “取消"按钮文本
	)

	if ok then
		log.trace("Execute code: " .. script_string)
		local code = load(script_string)
		if code then
			local ok, result = pcall(code)
			if ok then
				log.trace("Code execution OK. Result: " .. tostring(result))
			else
				gui.error_message(nil, _("Error occured when executing code: ") .. tostring(result))
				log.warn("Code execution failed. Error: " .. tostring(result))
			end
		else
			gui.error_message(nil, _("There are syntax errors in your code."))
			log.warn("Syntax error found in the code.")
		end
	end
end

-- 初始化菜单
-- event_data - 事件参数，table，包含以下值
--	main_window_handle - 主窗口的handle
--	main_menu_handle - 主菜单的handle
-- 返回值：一律返回0
function event.EVENT_INIT_MENU(event_data)
	log.debug("********************* on menuinit start *********************")

	-- 处理一下Tile Limit Breaker插件
	if plugin then
		if plugin.loaders['YDTileLimitBreaker'] and plugin.loaders['YDTileLimitBreaker'].start then
			plugin.loaders['YDTileLimitBreaker'].start()
		end
	end

	local menu = gui.menu(event_data.main_menu_handle, _("&YDWE"))
	menu:add(_("YDWE &Config"), launch_config)
	menu:add(_("Launch &Warcraft3"), launch_warcraft3)
	menu:add(_("Show las&t compile result"), show_last_error)
	menu:add(_("Show J&assHelper version"), show_jasshelper_version)
	menu:add(_("Show c&Jass version"), show_cjass_version)
	menu:add(_("Launch YDWE &official website"), open_offical_site)
	menu:add(_("&Export WE file"), file_export)
	menu:add(_("&Lua Test"), lua_test)
	menu:add(_("Cre&dits"), show_credit)

	-- 刷新菜单
	gui.draw_menu_bar(event_data.main_window_handle)

	main_window_handle = event_data.main_window_handle

	log.debug("********************* on menuinit end *********************")

	return 0
end
