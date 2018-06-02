local inject_code = require "compile.inject_code"
local wave = require "compile.wave"
local template = require "compile.template"
local cjass = require "compile.cjass"
local jasshelper = require "compile.jasshelper"
local native = require "compile.native"
local ev = require 'ev'
require "mpq_util"

-- 编译地图
-- map_path - 地图路径，string或者fs.path
-- option - 编译选项，table类型，支持的选项：
--	enable_preprocessor - 启用wave预处理器，true/false
-- 	enable_jasshelper - 启用JassHelper，true/false
--	enable_jasshelper_debug - 启用JassHelper的Debug，true/false
--	enable_jasshelper_optimization - 启用优化，true/false
--	enable_cjass - 启用cJass编译器，true/false
--	script_injection - 代码注入选项，整数，0表示智能注入，1表示不注入
--	runtime_version - 魔兽版本
-- 返回：true表示成功，false表示失败
local function compile_map(map_path, option)
	-- 统一处理fs.path
	if type(map_path) == "string" then map_path = fs.path(map_path) end
	-- 结果
	local result = nil

	log.trace("Save version " .. tostring(option.runtime_version))

	local compile_t = {
		['option'] = option,
		['map_path'] = map_path,
	}
	
	-- 如果JassHelper开启，执行正常编译
	if option.enable_jasshelper then
		result = mpq_util:update_file(map_path, "war3map.j", "1_war3map.j",
			-- 解压缩地图脚本，处理然后写回
			function (map_handle, in_script_path)
				-- 开始处理
				log.trace("Processing " .. in_script_path:filename():string())

				compile_t.input = in_script_path
				compile_t.output = nil
				compile_t.map_handle = map_handle
				compile_t.inject_file = function (file_path, path_in_archive)
					log.trace("[stormlib]import file", path_in_archive)
					return map_handle:add_file(path_in_archive, file_path)
				end
				
				-- 未启用用cJass
				if not option.enable_cjass then
					-- 根据注入选项进行处理（由于Lua的closure，此处可以访问“父”函数的局部变量）
					if option.script_injection == 0 then
						compile_t.output = fs.ydwe_path() / "logs" / "2_inject.j"
						if not inject_code:compile(compile_t) then
							return nil
						end
						compile_t.input = compile_t.output
					end

					-- Wave预处理
					compile_t.output = fs.ydwe_path() / "logs" / "3_wave.j"
					if not wave:compile(compile_t) then
						return nil
					end
					compile_t.input = compile_t.output
				end

                compile_t.output = fs.ydwe_path() / "logs" / "4_template.j"
				if not template:compile(compile_t) then
                    collectgarbage 'collect'
					return nil
				end
				ev.emit('编译地图')
                collectgarbage 'collect'
				return compile_t.output
			end
		)
		
		-- 开始调用编译工具编译
		if result then
			-- 调用cJass
			if option.enable_cjass then
				result = cjass:compile(map_path, option)
			end

			-- 调用jasshelepr
			if result then
				result = jasshelper:compile(map_path, option)
			end
		end

		native:compile(compile_t)
	else
		-- 只做语法检查
		result = jasshelper:compile(map_path, option)
	end

	return result
end

local compiler = {}

function compiler:compile(map_path, option)
    return compile_map(map_path, option)
end

function compiler:initialize()
	inject_code:initialize()
	native:initialize()
end

return compiler
