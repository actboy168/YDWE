local inject_code = require "compiler.inject_code"
local wave = require "compiler.wave"
local template = require "compiler.template"
local cjass = require "compiler.cjass"
local jasshelper = require "compiler.jasshelper"
local ev = require 'ev'
local stormlib = require 'ffi.stormlib'

local function update_script(map_path, path_tmp, process_function)
	-- 结果
	local result = false
	log.trace("Update mpq file")
    fs.copy_file(map_path / 'war3map.j', path_tmp, true)
    log.trace("war3map.j has been extracted from " .. map_path:filename():string())
    -- 调用处理函数处理
    local success, out_file_path = pcall(process_function, path_tmp)
    -- 如果函数正常结束（没有出错）
    if success then
        -- 如果函数成功完成任务
        if out_file_path then
            -- 替换文件
            fs.copy_file(out_file_path, map_path / 'war3map.j', true)
            result = true
        else
            -- 出现了错误
            log.error("Processor function cannot complete its task.")
        end
    else
        -- 记录出错原因
        log.error(out_file_path)
    end

	return result
end

local function make_option(config, war3ver)
	local option = {}
	-- 是否启用JassHelper
	option.enable_jasshelper = config.ScriptCompiler.EnableJassHelper ~= "0"
	-- 是否是调试模式
	option.enable_jasshelper_debug = config.ScriptCompiler.EnableJassHelperDebug == "1"
	-- 是否优化地图
	option.enable_jasshelper_optimization = config.ScriptCompiler.EnableJassHelperOptimization ~= "0"
	-- pjass的版本
	option.pjass = config.PJass.Option
	-- 代码注入选项
	-- cjass和脚本注入不能共存
	if option.enable_cjass then
		option.script_injection = false
	else
		option.script_injection = tonumber(config.ScriptInjection.Option)
	end
	-- 目标魔兽版本
	local save_type = tonumber(config.MapSave.Option)
	if save_type == 1 then
		-- 固定旧版本
		option.runtime_version = 20
	elseif save_type == 2 then
		-- 固定新版本
		option.runtime_version = 24
	else
		-- 按照当前版本或者双份
		option.runtime_version = war3ver
	end
	return option
end

local compiler = {}

function compiler:compile(map_path, config, war3ver)
    local option = make_option(config, war3ver)


	log.trace("Save version " .. tostring(option.runtime_version))

	local compile_t = {
		option = option,
        map_path = map_path,
        log = fs.ydwe_path() / "logs",
	}
	
    return update_script(map_path, compile_t.log / "1_war3map.j",
        -- 解压缩地图脚本，处理然后写回
        function (in_script_path)
            -- 开始处理
            log.trace("Processing " .. in_script_path:filename():string())

            compile_t.input = in_script_path
            compile_t.output = nil
            
            if option.enable_jasshelper then
                if option.script_injection == 0 then
                    compile_t.output = compile_t.log / "2_inject.j"
                    if not inject_code:compile(compile_t) then
                        return nil
                    end
                    compile_t.input = compile_t.output
                end

                -- Wave预处理
                compile_t.output = compile_t.log / "3_wave.j"
                if not wave:compile(compile_t) then
                    return nil
                end
                compile_t.input = compile_t.output

                compile_t.output = compile_t.log / "4_template.j"
                if not template:compile(compile_t) then
                    collectgarbage 'collect'
                    return nil
                end
                ev.emit('编译地图')
                collectgarbage 'collect'
            end

            compile_t.input = compile_t.output
            compile_t.output = compile_t.log / "6_vjass.j"
            if not jasshelper:compile(compile_t) then
                return nil
            end
            return compile_t.output
        end
    )
end

function compiler:initialize()
	inject_code:initialize()
end

return compiler
