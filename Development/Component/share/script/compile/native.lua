require "sys"
require "filesystem"
require "util"
require "mpq_util"

native = {}
native.list = {}
native.file = {}

local function slice_cut(slice, pattern)
	for i, s in ipairs(slice) do
		local f, l = s:find(pattern)
		if f then
			slice[i] = s:sub(1, f-1)
			table.insert(slice, i+1, s:sub(l+1))
			return true
		end
	end
	return false
end

function native:inject(input, output)
	local inbuf = io.load(input)
	if not inbuf then
		return false
	end
	local first, last = inbuf:find('endglobals\r')
	if not first then
		return false
	end
	local inject = {}
	local inglobal = inbuf:sub(1, first-1)
	local infunc = { inbuf:sub(last+1) }
	for native, file in pairs(self.list) do
		if slice_cut(infunc, native) then
			inject[file] = self.file[file]
		end
	end
	if not next(inject) then
		return false
	end
	local global = { }
	local func = { }
	for _, buf in pairs(inject) do
		local _, globals_first = buf:find('globals\r')
		local globals_last, function_first = buf:find('endglobals\r')
		if globals_first and globals_last then
			table.insert(global, buf:sub(globals_first+1, globals_last-1))
		else
			function_first = -1
		end
		table.insert(func, buf:sub(function_first+1))
	end

	local t = {}
	table.insert(t, inglobal)
	table.move(global, 1, #global, #t+1, t)
	table.insert(t, 'endglobals\r\n')
	table.move(func, 1, #func, #t+1, t)
	table.move(infunc, 1, #infunc, #t+1, t)
	return io.save(output, table.concat(t))
end

function native:compile(op)
	local native = fs.ydwe_path() / "jass" / "native"
	if not fs.exists(native) or not next(self.list) then
		return
	end
	log.info('Native compilation start.')
	return mpq_util:update_file(op.map_path, 'war3map.j',
		function (_, input)
			local output = fs.ydwe_path() / "logs" / "native.j"
			if not self:inject(input, output) then
				return input
			end
			return output
		end
	)
end

function native:scan(path)
	local buf = io.load(path)
	if not buf then
		return
	end
	local t = {}
	local pos = 1
	while true do
		local f, l = buf:find('native %w+ takes [%w, ]+', pos)
		if not f then
			table.insert(t, buf:sub(pos+1))
			break
		end
		local native = buf:sub(f, l)
		self.list[native] = path
		log.info('Native scan ', path, 'found', native)
		table.insert(t, buf:sub(pos, f-1))
		pos = l+1
	end
	self.file[path] = table.concat(t)
end

function native:initialize()
	local native = fs.ydwe_path() / "jass" / "native"
	if not fs.exists(native) then
		return
	end
	for fullpath in native:list_directory() do
		if fs.is_directory(fullpath) then
		elseif fullpath:extension():string() == ".j" then
			self:scan(fullpath)
		end
	end
end
