
local select=select
local setfenv=setfenv
local loadstring=loadstring
local string=string
local table=table
local stormlib = ar.stormlib

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

local function map_file_import(filename)
	return function (buf)
		local temp_file_path = fs.ydwe_path() / "logs" / "import" / filename
		fs.create_directories(temp_file_path:parent_path())
		if io.save(temp_file_path, buf) then
			if stormlib.add_file_ex(
				__map_handle__,
				temp_file_path,
				filename,
				bit.bor(stormlib.MPQ_FILE_COMPRESS, stormlib.MPQ_FILE_REPLACEEXISTING),
				stormlib.MPQ_COMPRESSION_ZLIB,
				stormlib.MPQ_COMPRESSION_ZLIB
			) then
				log.trace("import file succeeded.")
				return
			end
		end
		log.error("import file failed.")
	end
end

local function string_hash(str)
	return string.format('0x%08X', ar.storm.string_hash(str))
end

template = {}

local function sanbox(f, env) 
	local _ENV = env 
	return f	
end
	
function template.do_compile (self, code, env)
	local lua_codes = {''}
	table.insert(lua_codes, "local __jass_result__ = {''}")
	table.insert(lua_codes, "local function __jass_output__(str) table.insert(__jass_result__, str) end")
	local r, err = pcall(precompile, code, '__jass_output__', lua_codes)
	if not r then
		return r, err
	end
	table.insert(lua_codes, "return table.concat(__jass_result__)")	
	local f, err = loadstring(table.concat(lua_codes, '\n'))
	if not f then
		return f, err
	end
	
	return pcall(sanbox(f, env))
end

function template.compile(self, map_path, map_handle, map_script_path)
	log.trace("Template compilation start.")
    
	local output_path = fs.ydwe_path() / "logs" / "lua_processed.j"
	local content, err = io.load(map_script_path)
	if not content then
		log.error("Template read " .. map_script_path:string() .. ". Error: " .. err)
		return nil
	end

   	package.loaded['slk'] = nil
   	__map_handle__ = map_handle
	local env = setmetatable({import = map_file_import, StringHash = string_hash}, {__index = _G})
	local success, content = self:do_compile(content, env)
	if not success then
		if content then
			gui.message_dialog(nil, content, _("Error"), bit.bor(gui.MB_ICONERROR, gui.MB_OK))
		else
			gui.message_dialog(nil, _("Unknown"), _("Error"), bit.bor(gui.MB_ICONERROR, gui.MB_OK))
		end			
		log.error("Template error processing: " .. tostring(content))
		return nil
	end

	local result, err = io.save(output_path, content)
	if not result then
		log.error("Template write " .. output_path:string() .. ". Error: " .. err)
		return nil
	end
	
	log.debug("Template compilation succeeded.")
	return output_path
end
