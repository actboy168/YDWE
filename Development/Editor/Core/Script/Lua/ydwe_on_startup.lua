require "compile.inject_code"
require "mpq_util"

local storm    = ar.storm
local stormlib = ar.stormlib

-- 版本信息
ydwe_version = sys.version {}
war3_version = sys.war3_version {}

local function set_triggereditor_color()
	registry.SaveInt(registry.HKEY_CURRENT_USER, "Software\\Blizzard Entertainment\\WorldEdit\\Trigger Display Colors", "TC_YDHIDE",  0xffff0000)
	registry.SaveInt(registry.HKEY_CURRENT_USER, "Software\\Blizzard Entertainment\\WorldEdit\\Trigger Display Colors", "TC_COMMENT", 0xff008000)
end

-- 检查魔兽目录下是否有可能引起冲突的文件夹
local function check_conflicting_ui()
	log.trace("check_conflicting_ui")
	local file_list = {"ui/loading-yuexie.blp", "ui/miscdata.txt", "ui/triggerdata.txt", "ui/triggerstrings.txt", "ui/worldeditdata.txt", "ui/worldeditstrings.txt"}
	local found = false
	
	for index, file in ipairs(file_list) do
		if fs.exists(fs.war3_path() / file) then
			found = true
			break
		end
	end
	
	if found then
		if gui.message_dialog(nil, string.format(_("YDWE has detected that there is a directory named \"%s\" located in Warcraft 3 installation directory. It may prevent YDWE from working. Do you want to delete it?"), 'UI'), _("YDWE"), bit32.bor(gui.MB_ICONQUESTION, gui.MB_YESNO)) == gui.IDYES then
			for index, file in ipairs(file_list) do			
				log.debug("remove file " .. (fs.war3_path() / file):string())
				pcall(fs.remove_all, fs.war3_path() / file)
			end
		end
	end
end

local function check_conflicting_units()
	log.trace("check_conflicting_units")
	local units_dir = fs.war3_path() / 'Units'
	local found = false
	
	for file in units_dir:list_directory() do	
		if not fs.is_directory(file) then
			found = true
			break
		end
	end
	
	if found then
		if gui.message_dialog(nil, string.format(_("YDWE has detected that there is a directory named \"%s\" located in Warcraft 3 installation directory. It may prevent YDWE from working. Do you want to delete it?"), 'Units'), _("YDWE"), bit32.bor(gui.MB_ICONQUESTION, gui.MB_YESNO)) == gui.IDYES then		
			for file in units_dir:list_directory() do	
				if not fs.is_directory(file) then
					log.debug("remove file " .. file:string())
					pcall(fs.remove_all, file)
				end
			end
		end
	end
end

-- 清除可能引起冲突的文件
local function clear_potential_conflicting()
	log.trace("clear_potential_conflicting")
	-- 需要清理的文件列表
	local file_list = {"MiscData.txt", "TriggerData.txt", "TriggerStrings.txt", "WorldEditData.txt", "WorldEditLayout.txt", "WorldEditStrings.txt", "war3map.j", "blizzard.j", "Scripts/blizzard.j", "common.j", "Scripts/common.j"}

	for index, file in ipairs(file_list) do
		if fs.exists(fs.war3_path() / file) then
			log.debug("remove file " .. (fs.war3_path() / file):string())
			pcall(fs.remove, fs.war3_path() / file)
		end
	end
end

-- 从魔兽中得到是否是1.24。通过搜索common.j有没有导出StringHash
-- 返回值：两个，魔兽版本，错误信息。如果没有错误，错误信息为nil
local function get_war3_version_from_script()
	local err = "Cannot extract file from warcraft"
	local common_j_path = fs.ydwe_path() / "logs" / "common.j"
	local mpq_handle = stormlib.open_archive(fs.war3_path() / 'War3Patch.mpq', 0, ar.stormlib.MPQ_OPEN_READ_ONLY)
	if mpq_handle then
		if stormlib.has_file(mpq_handle, "common.j") then
			stormlib.extract_file(mpq_handle, common_j_path, "common.j")
		elseif stormlib.has_file(mpq_handle, "scripts\\common.j") then
			stormlib.extract_file(mpq_handle, common_j_path, "scripts\\common.j")
		else
			return war3_version, err
		end
		stormlib.close_archive(mpq_handle)
		
		local s, e = io.load(common_j_path)
		if not s then			
			return war3_version, e
		end
		
		if s:find("StringHash") then
			return war3_version:new(), nil
		else
			return war3_version:old(), nil
		end
	end

	return war3_version, err
end

-- 检测魔兽的版本
local function check_war3_version()
	log.trace("check_war3_version")

	-- 检查“版本转换器”等造成的game.dll和war3patch.mpq不一致的问题
	if global_config:get_integer("MapSave.Option", 0) == 0 then
		-- 检测魔兽中包含的脚本文件所代表的版本
		local script_war3_version, e = get_war3_version_from_script()

		if e then
			log.warn("Cannot get warcraft 3 version from script: " .. e)
		else
			-- 二者如果不一致，则提示
			if war3_version:is_new() ~= script_war3_version:is_new() then
				gui.message_dialog(
					nil,
					_("YDWE has detected that your game.dll and war3patch.mpq mismatch. It may be caused by the so called \"Warcraft Version Converter\". This situation will cause a failure on saving and testing maps. It is strongly recommended to use the offical patched provided by Blizzard."),
					_("YDWE"),
					bit32.bor(gui.MB_ICONQUESTION, gui.MB_OK)
				)
			end
		end
	end
end


-- 显示制作者和感谢信息
function show_credit()
	we.message_show("    ------------------------------------------------------------", 0)
	we.message_show("           Welcome to YDWE, the best WE mod in China!           ", 0)
	we.message_show("    ------------------------------------------------------------", 0)
	we.message_show("    Director:   Everguo", 0)
	we.message_show("    UI & Jass:  Fetrix_sai, Everguo, Warft_TigerCN, Wataloo & Xylign", 0)
	we.message_show("    Program:    Actboy168, Aeris & Warft_TigerCN", 0)
	we.message_show("    Art:        C kuhn", 0)
	we.message_show("    Official Site: http://www.ydwe.net", 0)
	we.message_show("    *** SPECIAL THANKS ***", 0)
	we.message_show("    JassNewGenPack for ideas at www.wc3c.net", 0)
	we.message_show("    Vexorian for his jasshelper compiler", 0)
	we.message_show("    ADOLF and VD for their cjass compiler & TESH", 0)
	we.message_show("    ...", 0)
	we.message_show("    And all users & supporters, including", 0)
	we.message_show("    YOU", 0)
end


-- 本函数在编辑器启动时调用，可以在本函数中载入一些插件
-- event_data - 事件参数
-- 	暂无内容
-- 返回值：返回非负数表示成功，负数表示失败
event.register(event.EVENT_WE_START, false, function (event_data)
	log.debug("********************* on startup start *********************")
	
	-- 读取版本
	ydwe_version = sys.version { file = fs.ydwe_path() / "ydwe.exe" }
	war3_version = sys.war3_version { file = fs.war3_path() / "game.dll" }

	log.debug("ydwe version " .. tostring(ydwe_version))
	log.debug("war3 version " .. tostring(war3_version))
	
	-- 刷新配置数据	
	global_config:reload()

	-- 检测UI和Units目录
	check_conflicting_ui()
	check_conflicting_units()

	-- 检查魔兽目录下是否有可能引起冲突的文件夹
	clear_potential_conflicting()

	-- 检测魔兽的版本
	check_war3_version()	

	-- 载入Patch MPQ
	mpq_util:load_mpq("units.mpq")
	mpq_util:load_mpq("ydwe.mpq")
	
	-- 加载插件
	plugin:load_all()

	-- 载入注入代码配置
	inject_code:initialize()
		
	set_triggereditor_color()

	-- 显示感谢信息
	show_credit()

	log.debug("********************* on startup end *********************")

	return 0
end)

-- 本函数在编辑器关闭时调用，可以在本函数中作一些清理工作
-- event_data - 事件参数。table类型，暂时没有内容。
-- 返回值：一定要返回0
event.register(event.EVENT_WE_EXIT, false, function (event_data)
	log.debug("********************* on exit start *********************")

	plugin:unload_all()

	log.debug("********************* on exit end *********************")

	return 0
end)

-- 本函数在测试地图时使用
-- event_data - 事件参数，table，包含以下值
--	map_path - 保存的地图路径，字符串类型
--	application_name - 应用程序名，字符串对象
--	command_line - 命令行，字符串对象
-- 返回：非负表示成功，负数表示失败。如成功且无特殊情况原则上应该返回0
event.register(event.EVENT_TEST_MAP, false, function (event_data)
	log.debug("********************* on test start *********************")

	-- 刷新配置数据
	global_config:reload()
	
	-- 获取当前测试的地图名
	local map_path = fs.path(event_data.map_path)
	log.debug("Testing " .. map_path:string())
	log.debug("Testing " .. event_data.command_line)

	-- 附加命令行
	local command_aux = ""

	-- 是否OpenGL方式？
	if global_config:get_integer("MapTest.LaunchOpenGL", 0) == 1 then
		command_aux = command_aux .. " -opengl"
	end

	-- 是否窗口方式？
	if global_config:get_integer("MapTest.LaunchWindowed", 1) == 1 then
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
end)

-- 在WE载入MSS引擎解码器时调用，过滤载入的dll
-- event_data - 事件参数，table，包含以下值
--	library_name - 解码器路径，字符串
-- 返回非负数表示允许载入，负数表示不允许。无特殊情况一般返回0

event.register(event.EVENT_MSS_LOAD,  false, function (event_data)
	log.debug("Loading provider " .. event_data.library_name)

	-- 全部放行
	return 0
end)
