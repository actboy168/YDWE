require "w3x2lni.check_object"
require 'w3x2lni.open_map'
local uiloader = require "uiloader"
local stormlib = require 'ffi.stormlib'


-- 版本信息
ydwe_version = sys.version {}
war3_version = sys.war3_version {}

local function initialize_reg()
    local registry = require "registry"
	local reg = registry.open [[HKEY_CURRENT_USER\Software\Blizzard Entertainment\WorldEdit]]
	-- 不弹用户协议
	reg["Has Been Run"] = { registry.REG_DWORD, 1 }
	if not reg["Visible UI Elements"] then
		-- 关掉刷子表
		reg["Visible UI Elements"] = { registry.REG_DWORD, 0x2B }
	end
	if not reg["New Map On Startup"] then
		-- 启动时不创建新地图
		reg["New Map On Startup"] = { registry.REG_DWORD, 0 }
	end
	-- 某些UI的颜色
	local reg = registry.open [[HKEY_CURRENT_USER\Software\Blizzard Entertainment\WorldEdit\Trigger Display Colors]]
	reg["TC_YDHIDE"] = { registry.REG_DWORD, 0xffff0000 }
	reg["TC_UNKNOWUI"] = { registry.REG_DWORD, 0xffff0000 }
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
		if gui.yesno_message(nil, string.format(LNG.DELETE_DANGER_DIR, 'UI'), 'YDWE') then
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
		if gui.yesno_message(nil, LNG.DELETE_DANGER_DIR, 'Units') then		
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
	local mpq = stormlib.open(fs.war3_path() / 'War3Patch.mpq', true)
	if mpq then
		if mpq:has_file("common.j") then
			mpq:extract("common.j", common_j_path)
		elseif mpq:has_file("scripts\\common.j") then
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

    if war3_version.major ~= 1 or war3_version.minor < 20 or war3_version.minor > 27 then
        log.error("Unsupported War3 %d.%d", war3_version.major, war3_version.minor)
        gui.error_message(nil, LNG.UNSUPPORTED_WAR3_VERSION, war3_version.major, war3_version.minor)
        os.exit(true, -1)
    end

	-- 检查“版本转换器”等造成的game.dll和war3patch.mpq不一致的问题
	if tonumber(global_config["MapSave"]["Option"]) == 0 then
		-- 检测魔兽中包含的脚本文件所代表的版本
		local script_war3_version, e = get_war3_version_from_script()

		if e then
			log.warn("Cannot get warcraft 3 version from script: " .. e)
		else
			-- 二者如果不一致，则提示
			if war3_version:is_new() ~= script_war3_version:is_new() then
				gui.error_message(nil, LNG.ERROR_WAR3_VERSION)
			end
		end
	end
end


-- 显示制作者和感谢信息
function show_credit()
	we.message_show("    ------------------------------------------------------------")
	we.message_show("           Welcome to YDWE, the best WE mod in China!           ")
	we.message_show("    ------------------------------------------------------------")
	we.message_show("    Author:    actboy168, sumneko")
	we.message_show("    Contribution:  aeris, everguo, fetrix_sai,")
	we.message_show("      h82258652, shawwwn, Warft_TigerCN, xylign")
	we.message_show("    Official Site: http://www.ydwe.net")
	we.message_show("    *** SPECIAL THANKS ***")
	we.message_show("    Vexorian for his jasshelper compiler")
	we.message_show("    Ladislav Zezula for his StormLib")
	we.message_show("    ...")
	we.message_show("    And all users & supporters, including")
	we.message_show("    YOU")
end


local function initialize_font()
	if global_config.Font.FontEnable ~= "1" then
		return
	end
	
	local name = global_config.Font.FontName
	local size = tonumber(global_config.Font.FontSize)
	if name == '' then name = 'system' end
	if size == nil then size = 12 end

	local font = (require 'ffi.font')(name, size)
	if font ~= 0 then
		log.info('Font: ', name, size)
		we.set_font(font)
	end
end

-- 从主程序的mpq目录下载入MPQ
-- mpqname - MPQ的文件名
-- 返回值：MPQ句柄
local function load_virtual_mpq(mpqname, priority)
	local mpq = fs.ydwe_path() / "share" / mpqname
	if fs.exists(mpq) then
		virtual_mpq.open_path(mpq, priority)
	end
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

	local suc, gitlog = pcall(require, 'gitlog')
	if suc then
		log.debug("commit: " .. gitlog.commit)
		log.debug("data: " .. gitlog.date)
	end
	
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
    load_virtual_mpq("mpq/war3", 14)
    local lang = (require "i18n").get_language()
    if lang ~= 'zh-CN' then
        load_virtual_mpq(lang .. "/mpq", 14)
    end
    load_virtual_mpq("zh-CN/mpq", 14)

	-- 加载插件
	plugin:load_all()
	
	-- 初始化UI加载器
	uiloader()
		
	initialize_reg()
	initialize_font()

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
	log.debug("Loading provider " .. event_data.library_name)

	-- 全部放行
	return 0
end

local event = require 'ev'
virtual_mpq.event(function(name, ...)
    event.emit('virtual_mpq: ' .. name, ...)
end)
