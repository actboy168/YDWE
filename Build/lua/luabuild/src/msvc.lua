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

local mt = {}

local need = { LIB = true, LIBPATH = true, PATH = true, INCLUDE = true, VCINSTALLDIR = true }

function mt:initialize(version)
    local msvc = os.getenv(('VS%dCOMNTOOLS'):format(version))
    if not msvc then
        return false
    end
    local f = io.popen(('"%s/vsvars32.bat" & set'):format(msvc), 'r')
    for line in f:lines() do
        local name, value = parse_env(line)
        local name = name:upper()
        if need[name] then
            if name ~= 'VCINSTALLDIR' then
                addenv(name, value)
            else
                self.path = value
            end
        end
    end
    f:close()
    if not self.path then
        return false
    end
    self.version = version
    return true
end

function mt:rebuild(solution, configuration, platform)
    execute(('MSBuild "%s" /m /v:m /t:rebuild /clp:ShowEventId /p:Configuration="%s",Platform="%s"'):format(solution:string(), configuration or 'Release', platform or 'Win32'))
end

return mt
