

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
	local config_program = fs.ydwe_path() / "ydwe.exe"
	local command_line = string.format('"%s" -war3', config_program:string())
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

-- 初始化菜单
-- event_data - 事件参数，table，包含以下值
--	main_window_handle - 主窗口的handle
--	main_menu_handle - 主菜单的handle
-- 返回值：一律返回0
function event.EVENT_INIT_MENU(event_data)
	log.debug("********************* on menuinit start *********************")

	local menu = gui.menu(event_data.main_menu_handle, LNG.MENU_YDWE)
	menu:add(LNG.MENU_CONIFG, launch_config)
	menu:add(LNG.MENU_LAUNCH_WAR3, launch_warcraft3)
	menu:add(LNG.MENU_SHOW_LAST_COMPILE_RESULT, show_last_error)
	menu:add(LNG.MENU_SHOW_JASSHELPER_VERSION, show_jasshelper_version)
	menu:add(LNG.MENU_SHOW_CJASS_VERSION, show_cjass_version)
	menu:add(LNG.MENU_OPEN_OFFICIAL_SITE, open_offical_site)
	menu:add(LNG.MENU_CREDITS, show_credit)

	main_window_handle = event_data.main_window_handle

	log.debug("********************* on menuinit end *********************")

	return 0
end
