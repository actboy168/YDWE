fs = require 'bee.filesystem'
local subprocess = require 'bee.subprocess'

local root = fs.ydwe_devpath()

local wave = {}
wave.path          = fs.ydwe_devpath() / "compiler" / "wave"
wave.sysinclude    = fs.ydwe_devpath() / "compiler" / "include"

-- 预处理代码
-- op.input - 输入文件路径
-- op.option - 预处理选项，table，支持的值有
-- 	runtime_version - 表示魔兽版本
-- 	enable_jasshelper_debug - 布尔值，是否是调试模式
-- 返回：number, info, path - 子进程返回值；预处理输出信息；输出文件路径
function wave:do_compile(op)
	local args = {}
	args[#args+1] = (self.path / "Wave.exe"):string()
	args[#args+1] = string.format('--output=%s',     op.output:string())
	args[#args+1] = string.format('--sysinclude=%s', self.sysinclude:string())
	args[#args+1] = string.format('--include=%s',    op.map_path:parent_path():string())
    for _, path in ipairs(require 'ui') do
        if fs.exists(path / 'jass') then
            args[#args+1] = string.format('--include=%s',    (path / 'jass'):string())
        end
    end
	args[#args+1] = string.format('--define=WARCRAFT_VERSION=%d', 100 + op.option.runtime_version)
	args[#args+1] = string.format('--define=YDWE_VERSION_STRING=\"%s\"', tostring(ydwe_version))
	if op.option.enable_jasshelper_debug then
		args[#args+1] = '--define=DEBUG=1'
	end
	if tonumber(global_config["ScriptInjection"]["Option"]) == 0 then
		args[#args+1] = "--define=SCRIPT_INJECTION=1"
	end
	if fs.exists(self.sysinclude / "WaveForce.i") then
		args[#args+1] = '--forceinclude=WaveForce.i'
	end
	args[#args+1] = "--extended"
	args[#args+1] = "--c99"
	args[#args+1] = "--preserve=2"
	args[#args+1] = "--line=0"
	args[#args+1] = op.input:string()

	local process = subprocess.spawn {
		args,
		stdout = true,
		stderr = true,
		hideWindow = true,
	}
	if not process then
		log.error(string.format("Executed %s failed", table.concat(args, " ")))
		return -1, nil, nil
	end
	log.trace(string.format("Executed %s.", table.concat(args, " ")))
    local out = process.stdout:read 'a'
    local err = process.stderr:read 'a'
    local exit_code = process:wait()
    return exit_code, out, err
end

function wave:compile(op)
	log.trace("Wave compilation start.")		
	
	local f = io.open(op.input, "a+b")
	if f then
		f:write("/**/\r\n")
		f:close()
	end
	local exit_code, out, err = self:do_compile(op)
	
    if out and out ~= '' then log.info('Wave stdout:', out) end
    if err and err ~= '' then log.error('Wave stderr:', err) end

	-- 退出码0代表成功
	if exit_code ~= 0 then
		if err then
			gui.error_message(nil, LNG.PREPEOCESSOR_FAILED, err)
		else
			gui.error_message(nil, LNG.PREPEOCESSOR_LAUNCH_FAILED)
		end
		return false
	end

	return true
end

return wave
