
local function single_test(commandline, mappath)
	return fs.war3_path():string() .. ' -loadfile "' .. mappath:string() .. '"' .. commandline
end

local function process_kills(proc)
	local list = process.list()
	local function kill_by_name(name)
		local ids = list[name:lower()]
		if ids ~= nil then
			for _, id in ipairs(ids) do
				process.kill(id)
			end
		end
	end
	if type(proc) == 'string' then
		kill_by_name(proc)
	elseif type(proc) == 'table' then
		for _, name in pairs(proc) do
			kill_by_name(name)
		end
	end
end

local function process_create(command_line, current_dir)
	local p = sys.process()
	p:hide_window()
	if not p:create(nil, command_line, current_dir) then
		log.error(string.format("Executed %s failed", command_line))
		return
	end
	p:close()
	p = nil
	log.trace(string.format("Executed %s.", command_line))
end

local function path_sub(a, b)
	local i = a
	local r = fs.path('')
	while i ~= '' and i ~= b do
		r = i:filename() / r
		i = i:parent_path()
	end
	return r
end

local function host_copy_dll(curdir)
	pcall(fs.copy_file, fs.ydwe_path() / 'bin' / 'msvcp120.dll', curdir / 'msvcp120.dll', true)
	pcall(fs.copy_file, fs.ydwe_path() / 'bin' / 'msvcr120.dll', curdir / 'msvcr120.dll', true)
	pcall(fs.copy_file, fs.ydwe_path() / 'bin' / 'StormLib.dll', curdir / 'StormLib.dll', true)
end

local function host_save_config(curdir, mappath)
	local reg = registry.current_user() / "Software\\Blizzard Entertainment\\Warcraft III\\String"
	local tbl = {
		--bot_mapcfgpath = '',
		bot_mappath = mappath:parent_path():string(),
		bot_defaultgamename = mappath:filename():string(),
		bot_defaultownername = reg["userlocal"],
		lan_war3version = war3_version.minor,
		map_path = path_sub(mappath, fs.war3_path()):string(),
		map_localpath = mappath:filename():string(),
	}

	if war3_version:is_new() then
		tbl.bot_mapcfgpath = (fs.ydwe_path() / "jass" / "system" / "ht"):string()
	else
		tbl.bot_mapcfgpath = (fs.ydwe_path() / "jass" / "system" / "rb"):string()
	end

	local str = ''
	for k, v in pairs(tbl) do
		str = str .. tostring(k) .. ' = ' .. tostring(v) .. '\n'
	end
	io.save(curdir / 'ydhost.cfg', str)
end

local function host_test(commandline, mappath)
	process_kills('ydhost')
	local curdir = fs.ydwe_path() / 'plugin' / 'ydhost'
	host_copy_dll(curdir)
	host_save_config(curdir, mappath)
	process_create(curdir / 'ydhost.exe', curdir)
	return fs.war3_path():string() .. commandline .. ' -auto'
end

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
	local mappath = fs.path(event_data.map_path)
	log.debug("Testing " .. mappath:string())
	log.debug("Testing " .. event_data.command_line)

	-- 附加命令行
	local commandline = ""

	-- 是否OpenGL方式？
	if global_config["MapTest"]["LaunchRenderingEngine"] == "OpenGL" then
		commandline = commandline .. " -opengl"
	end

	-- 是否窗口方式？
	if global_config["MapTest"]["LaunchWindowed"] ~= "0" then
		commandline = commandline .. " -window"
	end

	commandline = commandline .. ' -ydwe "' .. fs.ydwe_path():string() .. '"'

	log.debug("Testing " .. tostring(global_config["MapTest"]["EnableHost"]))
	if global_config["MapTest"]["EnableHost"] == "1" then
		commandline = host_test(commandline, mappath)
	else
		commandline = single_test(commandline, mappath)
	end

	local result = false
	-- 启动魔兽开始测试...
	local war3_helper_dll = fs.ydwe_path() / "plugin" / "warcraft3" / "yd_loader.dll"
	result = sys.spawn_inject(event_data.application_name, commandline, nil, war3_helper_dll)

	log.debug("********************* on test end *********************")
	if result then return 0 else return -1 end
end
