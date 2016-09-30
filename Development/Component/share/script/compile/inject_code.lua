
inject_code = {}

-- 注入代码表
inject_code.new_table = {}
inject_code.old_table = {}

function inject_code:inject_file(op, path_in_archive)
	op.inject_file(fs.ydwe_path() / "share" / "mpq" / "units" / path_in_archive, path_in_archive)
end

-- 侦测需要注入哪些代码
-- op.input - 脚本的路径，fs.path变量
-- op.option - 选项，table类型，支持成员：
-- 	runtime_version - 表示魔兽版本
-- 返回：一个table，数组形式，包含所有需要注入的文件名（注意不是fs.path）
function inject_code:detect(op)	
	-- 结果变量
	local inject_code = nil
	local inject_slk = false
	
	-- 读入所有文本
	local s, e = io.load(op.input)
	-- 文件存在
	if s then
		-- 结果
		inject_code = {}

		-- 检查是否有需要注入的函数
		local all_table = op.option.runtime_version:is_new() and self.new_table or self.old_table		
		local GeneralBounsSystemFile = fs.ydwe_path() / "jass" / "YDWEGeneralBounsSystem.j"

		for file, function_table in pairs(all_table) do	
		    if GeneralBounsSystemFile:string() == file:string() then
				for _, function_name in ipairs(function_table) do
					if s:find(function_name) then
					    inject_slk = true
						table.insert(inject_code, file)
						break
					end
				end
			else
				for _, function_name in ipairs(function_table) do
					if s:find(function_name) then
						table.insert(inject_code, file)
						break
					end
				end
			end
		end
	else
		log.error("Error occured when opening map script.")
		log.error(e)
	end

	if inject_slk then
		self:inject_file(op, "units\\abilitydata.slk")
		--会掉线
		--self:inject_file(op, "units\\abilitymetadata.slk")
	end
	
	return inject_code
end

-- 注入代码到Jass代码文件（最常见的是war3map.j）中
-- op.output - war3map.j的路径，fs.path对象
-- inject_code_path_table - 所有需要注入的代码文件路径，table，table中可以是
-- 		string - 此时为YDWE / "jass" 目录下的对应名称的文件
--		fs.path - 此时取其路径
-- 注：该table必须是数组形式的，哈希表形式的不处理
-- 返回值：0 - 成功；-1 - 出错失败；1 - 什么都没做
function inject_code:do_inject(op, inject_code_path_table)
	-- 结果
	local result = 1
	if inject_code_path_table and #inject_code_path_table > 0 then
		-- 默认成功
		result = 0
		log.trace("Writing code to " .. op.output:filename():string())

		-- 打开文件供写入（追加模式）
		local map_script_file, e = io.open(op.output, "a+b")
		if map_script_file then

			-- 循环处理每个需要注入的文件
			for index, inject_code_path in ipairs(inject_code_path_table) do
				local inject_code_path_string = nil
				if type(inject_code_path) == "string" then
					if inject_code_path:find("\\") or inject_code_path:find("/") then
						inject_code_path_string = fs.path(inject_code_path)
					else
						inject_code_path_string = fs.ydwe_path() / "jass" / inject_code_path
					end
				else
					inject_code_path_string = fs.path(inject_code_path)
				end

				log.trace("Injecting " .. inject_code_path_string:string())
				local code_content, e = io.load(inject_code_path_string)
				if code_content then
					-- 插入代码到原文件最后
					map_script_file:write(code_content)
					-- 写上一个换行符（我日他妈的cJass你居然不支持Linux格式的换行符）
					map_script_file:write("\r\n")
					-- 成功
					log.trace("Injection completed")
				else
					result = -1
					log.error("Error occured when reading code to inject.")
					log.error(e)
				end
			end
			
			-- 关闭文件
			map_script_file:close()
		else
			result = -1
			log.error("Error occured when writing code to map script")
			log.error(e)
		end
	end

	return result
end


function inject_code:compile(op)
	op.output = op.input
	return self:do_inject(op, self:detect(op))
end

-- 扫描注入代码
-- config_dir - 需要扫描的路径
-- 返回值无，修改全局变量inject_code_table_new以及inject_code_table_old
-- inject_code_table_new - 新版（1.24）函数表
-- inject_code_table_old - 旧版函数表
function inject_code:scan(config_dir)
	local counter = 0
	log.trace("Scanning for inject files in " .. config_dir:string())

	-- 遍历目录
	for  full_path in config_dir:list_directory() do		
		if fs.is_directory(full_path) then
			-- 递归处理
			counter = counter + self:scan(full_path)
		elseif full_path:extension():string() == ".cfg" then
			-- 插入新表
			local new_table = {}
			local old_table = {}

			-- 解析状态，默认0
			-- 0 - 1.24/1.20通用
			-- 1 - 1.24专用
			-- 2 - 1.20专用
			local state = 0

			-- 循环处理每一行
			for line in io.lines(full_path) do
				-- 插入函数名
				local trimed = line:trim()
				if trimed ~= "" and trimed:sub(1, 1) ~= "#" then
					if trimed == "[general]" then
						state = 0
					elseif trimed == "[new]" then
						state = 1
					elseif trimed == "[old]" then
						state = 2
					else
						if state == 0 then
							table.insert(new_table, trimed)
							table.insert(old_table, trimed)
						elseif state == 1 then
							table.insert(new_table, trimed)
						elseif state == 2 then
							table.insert(old_table, trimed)
						end
					end
				end
			end

			
			-- 插入全局表中（替换文件扩展名）
			local substitution = full_path
			substitution = substitution:replace_extension(fs.path(".j"))
			if #old_table > 0 then
				self.old_table[substitution] = old_table
			end

			if #new_table > 0 then
				self.new_table[substitution] = new_table
			end

			counter = counter + 1
		end
	end
	return counter
end

function inject_code:initialize()
	local counter = self:scan(fs.ydwe_path() / "jass")
	log.trace(string.format("Scanned file: %d", counter))
end
