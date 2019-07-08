

-- 主窗口句柄
local main_window_handle = nil

-- 启动配置对话框
local function launch_config()
	sys.spawn { 
		(fs.ydwe_path() / "bin" / "YDWEConfig.exe"):string(),
		cwd = fs.ydwe_path(),
	}
end

-- 启动魔兽
local function launch_warcraft3()
	sys.spawn { 
		(fs.ydwe_path() / "ydwe.exe"):string(),
		"-war3",
		cwd = fs.ydwe_path(),
	}
end

-- 打开官网
local function open_offical_site()
	os.execute('explorer "http://www.ydwe.net"')
end

local function open_thanks()
    local thanks = require 'gui_thanks'
    thanks()
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
	menu:add(LNG.MENU_OPEN_OFFICIAL_SITE, open_offical_site)
	menu:add(LNG.MENU_CREDITS, open_thanks)

	main_window_handle = event_data.main_window_handle

	log.debug("********************* on menuinit end *********************")

	return 0
end
