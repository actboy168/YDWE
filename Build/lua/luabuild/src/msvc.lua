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
    local path = reg[("%d.0"):format(math.ceil(version / 10))]
    return fs.path(path)
end

local mt = {}

local need = { LIB = true, LIBPATH = true, PATH = true, INCLUDE = true }

function mt:initialize(version)
    self.__path = msvc_path(version)
	local vsvars32 = self.__path / 'Common7' / 'Tools' / 'VsDevCmd.bat'
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

function mt:redistversion()
    local verfile = self.__path / 'VC' / 'Auxiliary' / 'Build' / 'Microsoft.VCRedistVersion.default.txt'
    local f = assert(io.open(uni.u2a(verfile:string()), 'r'))
	local r = f:read 'a'
    f:close()
    return strtrim(r)
end

function mt:crtpath()
    return self.__path / 'VC' / 'Redist' / 'MSVC' / self:redistversion() / 'x86' / ('Microsoft.VC' .. self.version .. '.CRT')
end

function mt:sdkpath()
	local reg = registry.open [[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows Kits\Installed Roots]]
    return fs.path(reg.KitsRoot10)
end

function mt:ucrtpath()
    return self:sdkpath() / 'Redist' / 'ucrt' / 'DLLs' / 'x86'
end

function mt:copy_crt_dll(target)
    fs.create_directories(target)
    fs.copy_file(self:crtpath() / 'msvcp140.dll', target / 'msvcp140.dll', true)
    fs.copy_file(self:crtpath() / 'vcruntime140.dll', target / 'vcruntime140.dll', true)
    for dll in self:ucrtpath():list_directory() do
        fs.copy_file(dll, target / dll:filename(), true)
    end
end

function mt:rebuild(solution, configuration, platform)
    execute(('MSBuild "%s" /m /v:m /t:rebuild /clp:ShowEventId /p:Configuration="%s",Platform="%s"'):format(solution:string(), configuration or 'Release', platform or 'Win32'))
end

return mt
