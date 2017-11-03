
local select=select
local load=load
local string=string
local table=table
local storm = require 'ffi.storm'

local function result(...)
	return select("#",...), select(1,...)
end

local function precompile(code, output_func, lua_codes)
	if lua_codes == nil then
		lua_codes = {''}
	end
	local start_tag = '<?'
	local end_tag = '?>'
	
	local start1, end1 = string.find(code, start_tag, 1, true)
	local start2 = nil
	local end2 = 0
	local cEqual = string.byte('=', 1)
	while start1 ~= nil do
		if start1 > end2 + 1 then
			table.insert(lua_codes, output_func..'('..string.format("%q", string.sub(code, end2 + 1, start1 - 1))..')')
		end
		start2, end2 = string.find(code, end_tag, end1 + 1, true)
		if start2 ~= nil then
			if string.byte(code, end1 + 1) ~= cEqual then
				table.insert(lua_codes, string.sub(code, end1 + 1, start2 - 1))
			else
				table.insert(lua_codes, output_func..'('..string.sub(code, end1 + 2, start2 - 1)..')')
			end
			start1, end1 = string.find(code, start_tag, end2 + 1, true)
		else
			-- end tag not found
			end2 = end1
			start1 = nil
			-- we could error it, but we ignore it here
		end
	end
	table.insert(lua_codes, output_func..'('..string.format("%q", string.sub(code, end2 + 1))..')')
	return 
end

local function map_file_import(path_in_archive)
	return function (buf, is_path)		
		if is_path then
			log.trace("[stormlib]import file", path_in_archive)
			__map_handle__:add_file(path_in_archive, __map_path__:parent_path() / buf)
			return
		else
			local temp_file_path = fs.ydwe_path() / "logs" / "import" / path_in_archive
			fs.create_directories(temp_file_path:parent_path())
			if not io.save(temp_file_path, buf) then
				log.error("failed: save " .. temp_file_path:string())
				return
			end
			log.trace("[stormlib]import file", path_in_archive)
			__map_handle__:add_file(path_in_archive, temp_file_path)
			return
		end
	end
end

local function string_hash(str)
	return string.format('0x%08X', storm.string_hash(str))
end

template = {}
	
function template:do_compile(op)
	local code, err = io.load(op.input)
	if not code then
		log.error("Template read " .. op.input:string() .. ". Error: " .. err)
		return false, err
	end
	
	local lua_codes = {''}
	table.insert(lua_codes, "local __jass_result__ = {''}")
	table.insert(lua_codes, "local function __jass_output__(str) table.insert(__jass_result__, str) end")
	local r, err = pcall(precompile, code, '__jass_output__', lua_codes)
	if not r then
		return r, err
	end
	
	package.loaded['slk'] = nil
	__map_handle__ = op.map_handle
	__map_path__   = op.map_path
	local env = {
		import = map_file_import, 
		StringHash = string_hash,
		ability2order = require 'computed.ability2order',
	}
	setmetatable(env, {__index = _G})
	table.insert(lua_codes, "return table.concat(__jass_result__)")	
	local f, err = load(table.concat(lua_codes, '\n'), nil, 't', env)
	if not f then
		return f, err
	end
	
	local suc, content = xpcall(f, function(msg) return debug.traceback(msg) end)
	if not suc then
    	return false, content
    end
    if package.loaded['slk'] then
        package.loaded['slk']:refresh(__map_handle__.handle)
    end
    return true, content
end

function template:compile(op)
	log.trace("Template compilation start.")
	op.output = fs.ydwe_path() / "logs" / "lua_processed.j"
	local success, content = self:do_compile(op)
	if not success then
		if content then
			local msg = content
			local pos = content:find 'stack traceback:'
			if pos then
				msg = msg:sub(1, pos-1)
			end
			gui.error_message(nil, __(msg))
		else
			gui.error_message(nil, _("Unknown"))
		end			
		log.error("Template error processing: " .. tostring(content))
		return false
	end

	local result, err = io.save(op.output, content)
	if not result then
		log.error("Template write " .. op.output:string() .. ". Error: " .. err)
		return false
	end
	
	log.debug("Template compilation succeeded.")
	return true
end
