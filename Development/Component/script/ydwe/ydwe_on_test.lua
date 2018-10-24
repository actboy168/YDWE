local stormlib = require 'ffi.stormlib'
local w3x2lni = require 'compiler.w3x2lni.init'
local ev = require 'ev'
local map_packer = require 'w3x2lni.map_packer'

local mapdump = require 'mapdump'
local subprocess = require 'subprocess'

local mt = {}
mt.__index = mt

function mt:process_create(app, current_dir)
	if self.h then
		self.h:kill()
		self.h = nil
	end

	local process = subprocess.spawn {
		command_line,
		cwd = current_dir,
		hideWindow = true,
	}
	if not process then
		log.error('Executed failed: ', app)
		return
	end
	log.trace('Executed: ', app)
	self.h = process
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
		local w2l = w3x2lni()
		local tbl = w2l:frontend_w3i(w3i)
		local n = 0
		if tbl.CONFIG.FIX_FORCE_SETTING == 0 then
			n = tbl.PLAYER.PLAYER_COUNT
		else
			for i = 1, tbl.PLAYER.PLAYER_COUNT do
    			if tbl['PLAYER'..i].TYPE == 1 then
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


local function single_test(mappath)
	local args = {}
	args[#args + 1] = (fs.ydwe_path() / 'ydwe.exe'):string()
	args[#args + 1] = "-war3"
	args[#args + 1] = "-loadfile"
	args[#args + 1] = mappath:string()
	return args
end

local function host_copy_dll(curdir)
	pcall(fs.copy_file, fs.ydwe_path() / 'bin' / 'vcruntime140.dll', curdir / 'vcruntime140.dll', true)
	pcall(fs.copy_file, fs.ydwe_path() / 'bin' / 'msvcp140.dll', curdir / 'msvcp140.dll', true)
end

local function host_save_config(curdir, mappath, autostart)
	local ver = global_config_war3_version()
	local jasspath
	if ver:is_new() then
		jasspath = fs.ydwe_path() / "compiler" / "jass" / "24"
	else
		jasspath = fs.ydwe_path() / "compiler" / "jass" / "20"
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
		bot_mappath = fs.relative(mappath, fs.war3_path()):string(),
		bot_mapcfgpath = 'map.cfg',
	}
	local str = ''
	for k, v in pairs(tbl) do
		str = str .. tostring(k) .. ' = ' .. tostring(v) .. '\n'
	end
	io.save(curdir / 'ydhost.cfg', str)
end

local function host_test(mappath)
	local args = {}
	local host_test = tonumber(global_config["HostTest"]["Option"])
	local curdir = fs.ydwe_path() / 'plugin' / 'ydhost'
	host_copy_dll(curdir)
	host_save_config(curdir, mappath, host_test + 1)
	ydhost:process_create(curdir / 'ydhost.exe', curdir)

	args[#args + 1] = (fs.ydwe_path() / 'ydwe.exe'):string()
	args[#args + 1] = "-war3"
	args[#args + 1] = "-auto"
	if host_test == 0 then
		return args, 1
	end
	return args, getplayernum(mappath)
end

local function is_lni(path)
    if path:filename():string() ~= '.w3x' then
        return false
    end
    local f = io.open(path)
    if not f then
        return false
    end
    if f:seek('set', 8) and 'W2L\x01' == f:read(4) then
        f:close()
        return true
    end
    f:close()
    return false
end

local current_map_path
ev.on('打开地图', function (map_path)
	current_map_path = fs.path(map_path)
end)

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
	log.info('Current map path ' .. current_map_path:string())
	log.debug("Testing " .. mappath:string())
    log.debug("Testing " .. event_data.command_line)
	
	-- 如果测试前没有保存过，则在这里转换地图
	if not event_data.save then
		local mapSlk = "0" ~= global_config["MapTest"]["EnableMapSlk"]
		-- 如果是lni地图，需要重新打包
		if is_lni(current_map_path) then
			if mapSlk then
				if not map_packer('slk', current_map_path, mappath) then
					log.inifo('Slk map failed!')
					return -1
				end
			else 
				if not map_packer('obj', current_map_path, mappath) then
					log.inifo('Pack map failed!')
					return -1
				end
			end
		else
			if mapSlk then
				if not map_packer('slk', current_map_path, mappath) then
					log.inifo('Slk map failed!')
					return -1
				end
			end
		end
	end

	-- 附加命令行
	local args, n

	log.debug("Testing " .. tostring(global_config["MapTest"]["EnableHost"]))
	if global_config["MapTest"]["EnableHost"] == "1" then
		args, n = host_test(mappath)
	else
		args, n = single_test(mappath), 1
	end
	args[#args + 1] = "-closew2l"

	local result = true
	for i = 1, n do
		if not subprocess.spawn(args) then
			log.error(string.format("Executed %s failed", table.concat(args, ' ')))
			result = false
			break
		end
		log.trace(string.format("Executed %s.", table.concat(args, ' ')))
	end

	log.debug("********************* on test end *********************")
	if result then return 0 else return -1 end
end
