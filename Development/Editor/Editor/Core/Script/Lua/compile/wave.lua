require "sys"
require "filesystem"
require "util"

wave = {}
wave.path                = fs.ydwe_path() / "plugin" / "wave"
wave.exe_path            = wave.path / "Wave.exe"
wave.config_path         = wave.path / "WaveConfig.cfg"
wave.sys_include_path    = wave.path / "include"
wave.plugin_include_path = fs.ydwe_path() / "plugin"
wave.jass_include_path   = fs.ydwe_path() / "jass"
wave.force_file_path     = wave.sys_include_path / "WaveForce.i"

-- 预处理代码
-- code_path - 输入文件路径
-- option - 预处理选项，table，支持的值有
-- 	runtime_version - 表示魔兽版本
-- 	enable_jasshelper_debug - 布尔值，是否是调试模式
--	enable_yd_trigger - 布尔值，是否启用YD触发器
-- 返回：number, info, path - 子进程返回值；预处理输出信息；输出文件路径
function wave.do_compile(self, code_path, option)
	-- 输出路径
	local out_file_path = code_path:parent_path() / (code_path:stem():string() .. ".i")

	-- 生成配置文件
	local f, e = io.open(self.config_path:string(), "w")
	if f then
		-- 写入内容
		f:write("--autooutput\n")
		f:write(string.format('--sysinclude="%s"\n', self.sys_include_path:string()))
		f:write(string.format('--sysinclude="%s"\n', self.plugin_include_path:string()))
		f:write(string.format('--include="%s"\n',    self.jass_include_path:string()))
		f:write(string.format('--define=WARCRAFT_VERSION=%d\n', 100 * option.runtime_version.major + option.runtime_version.minor))
		f:write(string.format('--define=YDWE_VERSION_STRING="%s"\n', tostring(ydwe_version)))
		if option.enable_jasshelper_debug then
			f:write('--define=DEBUG=1\n')
		end
		if global_config:get_integer("ScriptInjection.Option", 0) == 0 then
			f:write("--define=SCRIPT_INJECTION=1\n")
		end
		if not option.enable_yd_trigger then
			f:write('--define=DISABLE_YDTRIGGER=1\n')
		end
		if fs.exists(self.force_file_path) then
			f:write(string.format('--forceinclude=%s\n', self.force_file_path:filename():string()))
		end
		f:write("--extended\n--c99\n--preserve=2\n--line=0\n")
		f:close()

		local command_line = string.format('"%s" @"%s" "%s"', self.exe_path:string(), self.config_path:string(), code_path:string())
		-- 启动进程
		local proc, out_rd, err_rd, in_wr = sys.spawn_pipe(command_line, nil)
		if proc then
			local out = out_rd:read("*a")
			local err = err_rd:read("*a")
			local exit_code = proc:wait()				
			proc:close()
			proc = nil
			return exit_code, out, err, out_file_path
		else
			return -1, nil, nil, out_file_path
		end
	else
		return -1, nil, nil, out_file_path
	end
end

function wave.compile(self, code_path, option)
	log.trace("Wave compilation start.")		
	
	local exit_code, out, err, out_file_path = self:do_compile(code_path, option)
	
	-- 退出码0代表成功
	if exit_code ~= 0 then
		if out and err then
			local message = string.format(_("Preprocessor failed with message:\nstdout:%s\nstderr: %s"), out, err)
			gui.message_dialog(nil, message, _("Error"), bit.bor(gui.MB_ICONERROR, gui.MB_OK))
		else
			gui.message_dialog(nil, _("Cannot start preprocessor process."), _("Error"), bit.bor(gui.MB_ICONERROR, gui.MB_OK))
		end
		return nil
	end

	return out_file_path
end
