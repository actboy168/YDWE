local stormlib = require 'ffi.stormlib'
local w2l = require 'w3x2lni'
local mapdump = require 'mapdump'

local mt = {}
mt.__index = mt

function mt:process_create(command_line, current_dir)
	if self.h then
		self.h:kill()
		self.h = nil
	end

	local p = sys.process()
	p:hide_window()
	if not p:create(nil, command_line, current_dir) then
		log.error('Executed failed: ', command_line)
		return
	end
	log.trace('Executed: ', command_line)
	self.h = p
end

function mt:__gc()
	if self.h then
		self.h:kill()
		self.h = nil
	end
end

local ydhost = setmetatable({}, mt)

local function getplayernum(mappath)
	local ok, result = pcall(function()
		local map = stormlib.open(mappath, true)
		if not map then
    		return 0
		end
		local w3i = map:load_file('war3map.w3i')
		local tbl = w2l:read_w3i(w3i)
		local n = 0
		if tbl['选项']['自定义玩家分组'] == 0 then
			n = tbl['玩家']['玩家数量']
		else
			for i = 1, tbl['玩家']['玩家数量'] do
    			if tbl['玩家'..i]['类型'] == 1 then
					n = n + 1
				end
			end
		end
		map:close()
		return n
	end)
	if ok then return result end
	return 0
end


local function single_test(commandline, mappath)
	return '"' .. (fs.war3_path() / 'war3.exe'):string() .. '" -loadfile "' .. mappath:string() .. '"' .. commandline
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
	pcall(fs.copy_file, fs.ydwe_path() / 'bin' / 'vcruntime140.dll', curdir / 'vcruntime140.dll', true)
	pcall(fs.copy_file, fs.ydwe_path() / 'bin' / 'msvcp140.dll', curdir / 'msvcp140.dll', true)
end

local function get_war3_version()
	if global_config["War3Patch"]["Option"] == "1" then
		if global_config["MapSave"]["Option"] == "1" then
			return sys.war3_version {
				major    = 1,
				minor    = 20,
				revision = 4,
				build    = 6074,
			}
		else
			return sys.war3_version {
				major    = 1,
				minor    = 24,
				revision = 4,
				build    = 6387,
			}
		end
	elseif global_config["War3Patch"]["Option"] == "2" then
		return sys.war3_version {
			file = fs.ydwe_path() / 'share' / 'patch' / global_config["War3Patch"]["DirName"] / 'Game.dll'
		}
	end
	return war3_version
end

local function host_save_config(curdir, mappath, autostart)
	local ver = get_war3_version()
	local jasspath
	if ver:is_new() then
		jasspath = fs.ydwe_path() / "jass" / "system" / "ht"
	else
		jasspath = fs.ydwe_path() / "jass" / "system" / "rb"
	end
	local of = io.open(curdir / 'map.cfg', 'wb')
	local ok, e = pcall(mapdump, mappath, jasspath,
		function(s)
			of:write(s .. '\n') 
		end
	)
	of:close()
	if not ok then
		log.error('')
	end
	local tbl = {
		lan_war3version = ver.minor,
		bot_defaultgamename = mappath:filename():string(),
		bot_autostart = autostart,
		bot_mappath = path_sub(mappath, fs.war3_path()):string(),
		bot_mapcfgpath = 'map.cfg',
	}
	local str = ''
	for k, v in pairs(tbl) do
		str = str .. tostring(k) .. ' = ' .. tostring(v) .. '\n'
	end
	io.save(curdir / 'ydhost.cfg', str)
end

local function host_test(commandline, mappath)
	local host_test = tonumber(global_config["HostTest"]["Option"])
	local curdir = fs.ydwe_path() / 'plugin' / 'ydhost'
	host_copy_dll(curdir)
	host_save_config(curdir, mappath, host_test + 1)
	ydhost:process_create(curdir / 'ydhost.exe', curdir)
	local cmd = '"' .. (fs.war3_path() / 'war3.exe'):string() .. '"' .. commandline .. ' -auto'
	if host_test == 0 then
		return cmd, 1
	end
	return cmd, getplayernum(mappath)
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
	local n = 0

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
		commandline, n = host_test(commandline, mappath)
	else
		commandline, n = single_test(commandline, mappath), 1
	end

	local result = false
	-- 启动魔兽开始测试...
	for i = 1, n do
		local war3_helper_dll = fs.ydwe_path() / "plugin" / "warcraft3" / "yd_loader.dll"
		result = sys.spawn_inject(event_data.application_name, commandline, nil, war3_helper_dll)
	end

	log.debug("********************* on test end *********************")
	if result then return 0 else return -1 end
end
