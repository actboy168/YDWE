require "sys"
require "filesystem"


-- 根据ydwebase.dll的路径计算
fs.__ydwe_path = fs.get(fs.DIR_MODULE):remove_filename():remove_filename()
log.debug('ydwe path ' .. fs.__ydwe_path:string())

-- 根据ydweworldedit.exe的路径计算
fs.__war3_path = fs.get(fs.DIR_EXE):remove_filename()
log.debug('war3 path ' .. fs.__war3_path:string())


function fs.ydwe_path()
	return fs.__ydwe_path
end

function fs.war3_path()
	return fs.__war3_path
end


-- 给路径末尾，扩展名前添加内容
-- 效果：("abc.w3x", "def") -> "abcdef.w3x"
function fs.path.aux_filename(self, str)
	return self:parent_path() / (self:stem():string() .. str .. self:extension():string())
end

-- 载入一个文件的内容
-- file_path - 文件路径, 必须是fs.path类型
-- 返回文件内容, nil表示出错
function io.load(file_path)
	local f, e = io.open(file_path:string(), "rb")

	if f then
		local content = f:read("*a")
		f:close()
		return content
	else
		return nil, e
	end
end

-- 保存一个文件的内容
-- file_path - 文件路径, 必须是fs.path类型
-- content - 文件内容,必须是字符串
-- 返回true表示成功,false表示失败
function io.save(file_path, content)
	local f, e = io.open(file_path:string(), "wb")

	if f then
		f:write(content)
		f:close()
		return true
	else
		return false, e
	end
end

-- 比较2个浮点数是否相等
-- a, b - 要比较的2个浮点数
-- eps - 精确度阈值
-- 返回值：true表示2个数的差在精确阈值内（相等），false表明不在阈值内
function math.feq(a, b, eps)
	if not eps then eps = 1e-10 end
	return math.abs(a - b) < eps
end

function string.trim (self) 
	return self:gsub("^%s*(.-)%s*$", "%1")
end

function string.from_objectid (id)
	return string.pack('>I4', id)
end

function string.to_objectid (str)
	return string.unpack('>I4', str)
end

function sys.spawn_pipe (command_line, current_dir)		
	local in_rd,  in_wr  = sys.open_pipe()
	local out_rd, out_wr = sys.open_pipe()
	local err_rd, err_wr = sys.open_pipe()
	local p = sys.process()
	p:hide_window()
	p:redirect(in_rd, out_wr, err_wr)
	if not p:create(nil, command_line, current_dir) then
		log.error(string.format("Executed %s failed", command_line))
		return nil
	end	
	in_rd:close()
	out_wr:close()
	err_wr:close()
	log.trace(string.format("Executed %s.", command_line))
	return p, out_rd, err_rd, in_wr
end

function sys.spawn_inject (application, command_line, current_dir, inject_dll)		
	local p = sys.process()
	
	if inject_dll then
		if type(inject_dll) == "string" then
			p:inject(fs.path(inject_dll))
		else 
			p:inject(inject_dll)
		end
	end
	
	if not p:create(application, command_line, current_dir) then
		log.error(string.format("Executed %s failed", command_line))
		return false
	end
	
	p:close()
	p = nil
	log.trace(string.format("Executed %s.", command_line))
	return true
end

function sys.spawn (command_line, current_dir, wait)	
	local p = sys.process()
	if not p:create(nil, command_line, current_dir) then
		log.error(string.format("Executed %s failed", command_line))
		return false
	end

	if wait then
		local exit_code = p:wait()
		p:close()
		p = nil
		log.trace(string.format("Executed %s, returned %d", command_line, exit_code))
		return exit_code == 0
	end
	
	p:close()
	p = nil	
	log.trace(string.format("Executed %s.", command_line))
	return false
end

function sys.ini_load (path)
	local f = io.open(path, "r")
	local tbl = {}
	local section = nil
	if f then
		for line in f:lines() do
			if string.sub(line,1,1) == "[" then
				section = string.trim(string.sub(line, 2, string.len(line) - 1 ))
				tbl[section] = setmetatable({}, {__index = function () return '' end })
			else
				if string.trim(line) ~= "" then
					local key = string.trim(string.sub(line, 1, string.find(line, "=") - 1))
					local value = string.sub(line, string.find(line, "=") + 1)
					tbl[section][key] = value or ""
				end
			end
		end
		f:close()
	end
	return setmetatable(tbl, {__index = function () return '' end })
end

function sys.ini_save (path, tbl)
	local f = io.open(path, "w")
	for section, kv in pairs(tbl) do
		f:write("[" .. section .. "]\n")
		for key, value in pairs(kv) do
			f:write(string.trim(item) .. " = " .. string.trim(value) .. "\n")
		end
	end
	f:close()
end
