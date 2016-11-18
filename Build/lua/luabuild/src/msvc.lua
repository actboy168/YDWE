require 'registry'
local uni = require 'unicode'
local ffi = require 'ffi'
ffi.cdef[[
    int SetEnvironmentVariableA(const char* name, const char* value);
]]

local function strtrim(str) 
	return str:gsub("^%s*(.-)%s*$", "%1")
end

local function parse_env(str)
    local pos = str:find('=')
    if not pos then
        return nil, nil
    end
	return strtrim(str:sub(1, pos - 1)), strtrim(str:sub(pos + 1))
end

local function addenv(name, newvalue)
    local oldvalue = os.getenv(name)
    if oldvalue then
        newvalue = newvalue .. ';' .. oldvalue
    end
    ffi.C.SetEnvironmentVariableA(name, newvalue)
end

local function execute(command)	
    local f = io.popen(command, 'r')
    for line in f:lines() do
        print(line)
    end
    local ok = f:close()
    if not ok then
        error(("execute failed: %q"):format(command))
    end
end	

local function msvc_path(version)
    local reg = registry.local_machine() / [[SOFTWARE\Microsoft\VisualStudio\SxS\VS7]]
    local path = reg[("%d.0"):format(version // 10)]
    return fs.path(path)
end

local mt = {}

local need = { LIB = true, LIBPATH = true, PATH = true, INCLUDE = true }

function mt:initialize(version)
    self.__path = msvc_path(version)
	local vsvars32
    if version >= 150 then
	    vsvars32 = self.__path / 'Common7' / 'Tools' / 'VsDevCmd.bat'
	else
	    vsvars32 = self.__path / 'Tools' / 'vsvars32.bat'
    end
    local f = io.popen(('"%s" & set'):format(vsvars32:string()), 'r')
    for line in f:lines() do
        local name, value = parse_env(line)
        if name and value then
        	local name = name:upper()
        	if need[name] then
        	    addenv(name, value)
        	end
    	end
    end
    f:close()
    self.version = version
    return true
end

function mt:fullversion()
    local verfile = self.__path / 'VC' / 'Auxiliary' / 'Build' / 'Microsoft.VCToolsVersion.default.txt'
    local f = assert(io.open(uni.u2a(verfile:string()), 'r'))
	local r = f:read 'a'
    f:close()
    return strtrim(r)
end

function mt:crtpath()
    if self.version >= 150 then
	    return self.__path / 'VC' / 'Redist' / 'MSVC' / self:fullversion() / 'x86' / ('Microsoft.VC' .. self.version .. '.CRT')
    else
	    return self.__path / 'VC' / 'Redist' / 'x86' / ('Microsoft.VC' .. self.version .. '.CRT')
    end
end

function mt:rebuild(solution, configuration, platform)
    execute(('MSBuild "%s" /m /v:m /t:rebuild /clp:ShowEventId /p:Configuration="%s",Platform="%s"'):format(solution:string(), configuration or 'Release', platform or 'Win32'))
end

return mt
