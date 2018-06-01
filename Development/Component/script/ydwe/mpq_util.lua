
local storm    = require 'virtual_storm'
local stormlib = require 'ffi.stormlib'

mpq_util = {}

function mpq_util:update_file(map_path, path_in_archive, path_tmp, process_function)
	-- 结果
	local result = false
	log.trace("mpq_util.update_file.")

	-- 打开MPQ（地图）
	local mpq = stormlib.open(map_path)
	if mpq then
		-- 确定解压路径
		local extract_file_path = fs.ydwe_path() / "logs" / path_tmp
		-- 将文件解压
		if mpq:has_file(path_in_archive) and
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
					result = mpq:add_file(path_in_archive, out_file_path)
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
	local mpq = fs.ydwe_path() / "share" / mpqname
	if fs.exists(mpq) then
		virtual_mpq.open_path(mpq, priority)
	end
end
