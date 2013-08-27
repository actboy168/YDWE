
local storm    = ar.storm
local stormlib = ar.stormlib

mpq_util = {}

-- 添加文件给地图
-- map_path - 地图路径，fs.path
-- file_path - 需要加入的文件的路径，fs.path
-- path_in_archive - 地图压缩包中的路径，string
-- 返回值：true表示成功，false表示失败
function mpq_util.insert_file(self, map_path, file_path, path_in_archive)
	log.trace("mpq_util.insert_file.")
	-- 结果
	local result = false

	-- 打开MPQ（地图）
	local mpq_handle = stormlib.open_archive(map_path, 0, 0)
	if mpq_handle then
		if stormlib.add_file_ex(
			mpq_handle,
			file_path,
			path_in_archive,
			bit.bor(bit.bor(stormlib.MPQ_FILE_SINGLE_UNIT, stormlib.MPQ_FILE_COMPRESS), stormlib.MPQ_FILE_REPLACEEXISTING)
			stormlib.MPQ_COMPRESSION_ZLIB,
			stormlib.MPQ_COMPRESSION_ZLIB
		) then
			result = true
			log.error("insert_file .")
		else
			log.error("insert_file " .. map_path:string() .. " faild. ")
		end

		-- 关闭地图
		stormlib.close_archive(mpq_handle)
	else
		log.error("Cannot open map archive " .. map_path)
	end

	return result
end

-- 添加文件给地图
-- map_path - 地图路径，fs.path
-- path_in_archive - 地图压缩包中的路径，string
-- 返回值：true表示成功，false表示失败
function mpq_util.insert_file_form_ydwe(self, map_path, path_in_archive)
	log.trace("mpq_util.insert_file_form_ydwe.")
	local result = false
	local extract_file_path = fs.ydwe_path() / "logs" / path_in_archive	
	fs.create_directories(extract_file_path:parent_path())
	if storm.extract_file(extract_file_path, path_in_archive) then
		result = self:insert_file(map_path, extract_file_path, path_in_archive)

		-- 删除临时文件
		pcall(fs.remove_all, extract_file_path)
	else
		log.error("Cannot extract " .. path_in_archive)
	end
	return result
end

-- 从地图中解压出文件来然后调用回调函数更新
-- map_path - 地图路径，fs.path
-- path_in_archive - 地图压缩包中的路径，string
-- process_function - 函数，必须接收一个fs.path对象，返回一个fs.path对象
-- 形如 function (in_path) return out_path end
-- 返回值：true表示成功，false表示失败
function mpq_util.update_file(self, map_path, path_in_archive, process_function)
	-- 结果
	local result = false
	log.trace("mpq_util.update_file.")

	-- 打开MPQ（地图）
	local mpq_handle = stormlib.open_archive(map_path, 0, 0)
	if mpq_handle then
		-- 确定解压路径
		local extract_file_path = fs.ydwe_path() / "logs" / "file.out"
		-- 将文件解压
		if stormlib.has_file(mpq_handle, path_in_archive) and
			stormlib.extract_file(mpq_handle, extract_file_path, path_in_archive)
		then
			log.trace(path_in_archive .. " has been extracted from " .. map_path:filename():string())

			-- 调用处理函数处理
			local success, out_file_path = pcall(process_function, mpq_handle, extract_file_path)
			-- 如果函数正常结束（没有出错）
			if success then
				-- 如果函数成功完成任务
				if out_file_path then
					-- 替换文件
					if stormlib.add_file_ex(
						mpq_handle,
						out_file_path,
						path_in_archive,
						bit.bor(bit.bor(stormlib.MPQ_FILE_SINGLE_UNIT, stormlib.MPQ_FILE_COMPRESS), stormlib.MPQ_FILE_REPLACEEXISTING)
						stormlib.MPQ_COMPRESSION_ZLIB,
						stormlib.MPQ_COMPRESSION_ZLIB
					) then
						log.trace("Archive update succeeded.")
						result = true
					else
						log.error("Error occurred when write back")
					end
				else
					-- 出现了错误--
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
		stormlib.close_archive(mpq_handle)
	else
		log.error("Cannot open map archive" .. map_path:string())
	end

	return result
end

-- 从主程序的mpq目录下载入MPQ
-- mpqname - MPQ的文件名
-- 返回值：MPQ句柄
function mpq_util.load_mpq(self, mpqname)
	local result = 0
	local mpq = fs.ydwe_path() / "share" / "mpq" / mpqname

	-- 文件存在否？
	if fs.exists(mpq) then
		result = storm.open_archive(mpq, 14)
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
