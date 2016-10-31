
local storm    = ar.storm
local stormlib = require 'ffi.stormlib'

mpq_util = {}

local stormlib_mt =  {}
stormlib_mt.__index = {}

function stormlib_mt.__index:import(path_in_archive, file_path)
	log.trace("[stormlib]import file.")
	local suc = self.handle:add_file(path_in_archive, file_path)
	if suc then
		log.trace("succeeded: import " .. path_in_archive)
		return true
	else
		log.error("failed: import " .. file_path:string())
		return false
	end
	return suc
end

function stormlib_mt.__index:extract(path_in_archive, file_path)
	local dir = file_path:parent_path()
	if not fs.exists(dir) then
		fs.create_directories(dir)
	end
	return self.handle:extract(path_in_archive, file_path)
end

function stormlib_mt.__index:has(path_in_archive)
	return self.handle:has_file(path_in_archive)
end

function stormlib_mt.__index:load(path_in_archive)
	return self.handle:load_file(path_in_archive)
end

function stormlib_mt.__index:close()
	self.handle:close()
end

function mpq_util:stormlib(path, readonly)
	local obj = {}
	obj.handle = stormlib.open(path, readonly)
	if not obj.handle then
		return nil
	end
	return setmetatable(obj, stormlib_mt)
end

-- 从地图中解压出文件来然后调用回调函数更新
-- map_path - 地图路径，fs.path
-- path_in_archive - 地图压缩包中的路径，string
-- process_function - 函数，必须接收一个fs.path对象，返回一个fs.path对象
-- 形如 function (in_path) return out_path end
-- 返回值：true表示成功，false表示失败
function mpq_util:update_file(map_path, path_in_archive, process_function)
	-- 结果
	local result = false
	log.trace("mpq_util.update_file.")

	-- 打开MPQ（地图）
	local mpq = mpq_util:stormlib(map_path)
	if mpq then
		-- 确定解压路径
		local extract_file_path = fs.ydwe_path() / "logs" / "file.out"
		-- 将文件解压
		if mpq:has(path_in_archive) and
			mpq:extract(path_in_archive, extract_file_path)
		then
			log.trace(path_in_archive .. " has been extracted from " .. map_path:filename():string())

			-- 调用处理函数处理
			local success, out_file_path = pcall(process_function, mpq, extract_file_path)
			-- 如果函数正常结束（没有出错）
			if success then
				-- 如果函数成功完成任务
				if out_file_path then
					-- 替换文件
					result = mpq:import(path_in_archive, out_file_path)
				else
					-- 出现了错误
					log.error("Processor function cannot complete its task.")
				end
			else
				-- 记录出错原因
				log.error(out_file_path)
			end

			-- 删除临时文件
			--pcall(fs.remove_all, extract_file_path)
		else
			log.error("Cannot extract " .. path_in_archive)
		end

		-- 关闭地图
		mpq:close()
	else
		log.error("Cannot open map archive " .. map_path:string())
	end

	return result
end

-- 从主程序的mpq目录下载入MPQ
-- mpqname - MPQ的文件名
-- 返回值：MPQ句柄
function mpq_util:load_mpq(mpqname, priority)
	local result = 0
	local mpq = fs.ydwe_path() / "share" / "mpq" / mpqname

	if fs.exists(mpq) then
		virtual_mpq.open_path(mpq, priority)
		return result
	end
	
	mpq = fs.ydwe_path() / "share" / "mpq" / (mpqname .. ".mpq")
	if fs.exists(mpq) then
		result = storm.open_archive(mpq, priority)
		if result then
			log.debug("Loaded " .. mpq:filename():string())
		else
			log.error("Cannot load " .. mpq:filename():string())
		end
	else
		log.error("Cannot find " .. mpq:filename():string())
	end

	return result
end
