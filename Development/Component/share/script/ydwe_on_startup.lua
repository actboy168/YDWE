require "compile.inject_code"
require "compile.native"
require "mpq_util"

local storm    = ar.storm

-- 版本信息
ydwe_version = sys.version {}
war3_version = sys.war3_version {}

local function set_triggereditor_color()
	local reg = registry.current_user() / "Software\\Blizzard Entertainment\\WorldEdit\\Trigger Display Colors"
	reg["TC_YDHIDE"] = { registry.REG_DWORD, 0xffff0000 }
	reg["TC_COMMENT"] = { registry.REG_DWORD, 0xff008000 }
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
		if gui.yesno_message(nil, string.format(_("YDWE has detected that there is a directory named \"%s\" located in Warcraft 3 installation directory. It may prevent YDWE from working. Do you want to delete it?"), 'UI'), _("YDWE")) then
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
		if gui.yesno_message(nil, _("YDWE has detected that there is a directory named \"%s\" located in Warcraft 3 installation directory. It may prevent YDWE from working. Do you want to delete it?"), 'Units') then		
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
	local mpq = mpq_util:stormlib(fs.war3_path() / 'War3Patch.mpq', true)
	if mpq then
		if mpq:has("common.j") then
			mpq:extract("common.j", common_j_path)
		elseif mpq:has("scripts\\common.j") then
			mpq:extract("scripts\\common.j", common_j_path)
		else
			return war3_version, err
		end
		mpq:close()
		
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
	if tonumber(global_config["MapSave"]["Option"]) == 0 then
		-- 检测魔兽中包含的脚本文件所代表的版本
		local script_war3_version, e = get_war3_version_from_script()

		if e then
			log.warn("Cannot get warcraft 3 version from script: " .. e)
		else
			-- 二者如果不一致，则提示
			if war3_version:is_new() ~= script_war3_version:is_new() then
				gui.error_message(nil, _("YDWE has detected that your game.dll and war3patch.mpq mismatch. It may be caused by the so called \"Warcraft Version Converter\". This situation will cause a failure on saving and testing maps. It is strongly recommended to use the offical patched provided by Blizzard."))
			end
		end
	end
end


-- 显示制作者和感谢信息
function show_credit()
	we.message_show("    ------------------------------------------------------------")
	we.message_show("           Welcome to YDWE, the best WE mod in China!           ")
	we.message_show("    ------------------------------------------------------------")
	we.message_show("    Program:    Actboy168, Aeris")
	we.message_show("    UI & Jass:  Everguo, Fetrix_sai, Syj2010syj, Warft_TigerCN, Xylign")
	we.message_show("    Art:        C kuhn")
	we.message_show("    Official Site: http://www.ydwe.net")
	we.message_show("    *** SPECIAL THANKS ***")
	we.message_show("    JassNewGenPack for ideas at www.wc3c.net")
	we.message_show("    Vexorian for his jasshelper compiler")
	we.message_show("    ADOLF and VD for their cjass compiler & TESH")
	we.message_show("    ...")
	we.message_show("    And all users & supporters, including")
	we.message_show("    YOU")
end


-- 本函数在编辑器启动时调用，可以在本函数中载入一些插件
-- event_data - 事件参数
-- 	暂无内容
-- 返回值：返回非负数表示成功，负数表示失败
function event.EVENT_WE_START(event_data)
	log.debug("********************* on startup start *********************")
	
	-- 读取版本
	ydwe_version = sys.version { file = fs.ydwe_path() / "ydwe.exe" }
	war3_version = sys.war3_version { file = fs.war3_path() / "game.dll" }

	log.debug("ydwe version " .. tostring(ydwe_version))
	log.debug("war3 version " .. tostring(war3_version))
	
	-- 刷新配置数据	
	global_config_reload()

	-- 检测UI和Units目录
	check_conflicting_ui()
	check_conflicting_units()

	-- 检查魔兽目录下是否有可能引起冲突的文件夹
	clear_potential_conflicting()

	-- 检测魔兽的版本
	check_war3_version()	

	-- 载入Patch MPQ
	mpq_util:load_mpq("units", 14)
	mpq_util:load_mpq("war3", 14)

	-- 加载插件
	plugin:load_all()

	-- 初始化UI加载器
	uiloader:initialize()
	
	-- 载入注入代码配置
	inject_code:initialize()
	native:initialize()
		
	set_triggereditor_color()

	-- 显示感谢信息
	show_credit()

	log.debug("********************* on startup end *********************")

	return 0
end

-- 本函数在编辑器关闭时调用，可以在本函数中作一些清理工作
-- event_data - 事件参数。table类型，暂时没有内容。
-- 返回值：一定要返回0
function event.EVENT_WE_EXIT(event_data)
	log.debug("********************* on exit start *********************")

	plugin:unload_all()

	log.debug("********************* on exit end *********************")

	return 0
end

-- 在WE载入MSS引擎解码器时调用，过滤载入的dll
-- event_data - 事件参数，table，包含以下值
--	library_name - 解码器路径，字符串
-- 返回非负数表示允许载入，负数表示不允许。无特殊情况一般返回0

function event.EVENT_MSS_LOAD(event_data)
	log.debug("Loading provider " .. ansi_to_utf8(event_data.library_name))

	-- 全部放行
	return 0
end
