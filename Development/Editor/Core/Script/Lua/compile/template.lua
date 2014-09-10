
local select=select
local setfenv=setfenv
local load=load
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

local function map_file_import(path_in_archive)
	return function (buf, is_path)		
		if is_path then
			mpq_util:import_file(__map_handle__, __map_path__:parent_path() / buf, path_in_archive)
			return
		else
			local temp_file_path = fs.ydwe_path() / "logs" / "import" / path_in_archive
			fs.create_directories(temp_file_path:parent_path())
			if not io.save(temp_file_path, buf) then
				log.error("failed: save " .. temp_file_path:string())
				return
			end
			mpq_util:import_file(__map_handle__, temp_file_path, path_in_archive)
			return
		end
	end
end

local function string_hash(str)
	return string.format('0x%08X', ar.storm.string_hash(str))
end

template = {}
	
function template:do_compile(op)
	local code, err = io.load(op.input)
	if not code then
		log.error("Template read " .. op.input:string() .. ". Error: " .. err)
		return false, err
	end
	
	local lua_codes = {''}
	table.insert(lua_codes, [[
		stormlib = ar.stormlib
    
	    local file_path = fs.ydwe_path() / "logs" / "mu.out"

	    local update_j = {}
	    
	    local function update(file_name, func)
	        if file_name == 'war3map.j' then
	            table.insert(update_j, func)
	        else
	            if stormlib.has_file(__map_handle__, file_name) and
	                stormlib.extract_file(__map_handle__, file_path, file_name)
	            then
	                local content = func(io.load(file_path))
	                if content then
	                    io.save(file_path, content)
	                   	mpq_util:import_file(__map_handle__, file_path, file_name)
                	end
	            end
	        end
	    end

	    local function do_update_j(content)
	    	for _, func in ipairs(update_j) do
		    	content = func(content) or content
	    	end
	    	return content
    	end
	]])
	table.insert(lua_codes, "local __jass_result__ = {''}")
	table.insert(lua_codes, "local function __jass_output__(str) table.insert(__jass_result__, str) end")
	local r, err = pcall(precompile, code, '__jass_output__', lua_codes)
	if not r then
		return r, err
	end
	
	package.loaded['slk'] = nil
	__map_handle__ = op.map_handle
	__map_path__   = op.map_path
	local env = setmetatable({import = map_file_import, StringHash = string_hash}, {__index = _G})
	table.insert(lua_codes, "return do_update_j(table.concat(__jass_result__))")	
	local f, err = load(table.concat(lua_codes, '\n'), nil, 't', env)
	if not f then
		return f, err
	end
	
	return pcall(f)
end

function template:compile(op)
	log.trace("Template compilation start.")
	op.output = fs.ydwe_path() / "logs" / "lua_processed.j"
	local success, content = self:do_compile(op)
	if not success then
		if content then
			gui.message_dialog(nil, content, _("Error"), bit32.bor(gui.MB_ICONERROR, gui.MB_OK))
		else
			gui.message_dialog(nil, _("Unknown"), _("Error"), bit32.bor(gui.MB_ICONERROR, gui.MB_OK))
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
