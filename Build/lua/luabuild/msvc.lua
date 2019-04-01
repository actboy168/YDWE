local fs = require 'bee.filesystem'
local sp = require 'bee.subprocess'
local registry = require 'bee.registry'
local uni = require 'bee.unicode'
local ffi = require 'ffi'
ffi.cdef[[
    int SetEnvironmentVariableA(const char* name, const char* value);
]]

local vswhere = fs.path(os.getenv('ProgramFiles(x86)')) / 'Microsoft Visual Studio' / 'Installer' / 'vswhere.exe'


local function strtrim(str) 
    return str:gsub("^%s*(.-)%s*$", "%1")
end

local function installpath()
    local process = assert(sp.spawn {
        vswhere,
        '-latest',
        '-products', '*',
        '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
        '-property', 'installationPath',
        stdout = true,
    })
    local result = strtrim(process.stdout:read 'a')
    process.stdout:close()
    process:wait()
    assert(result ~= "", "can't find msvc.")
    return fs.path(result)
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

local mt = {}

local need = { LIB = true, LIBPATH = true, PATH = true, INCLUDE = true }

function mt:initialize(version, coding)
    self.__path = installpath()
    if not self.__path then
        return false
    end
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
    self.coding = coding or 'ansi'
    return true
end

function mt:redistversion()
    local verfile = self.__path / 'VC' / 'Auxiliary' / 'Build' / 'Microsoft.VCRedistVersion.default.txt'
    local f = assert(io.open(verfile:string(), 'r'))
    local r = f:read 'a'
    f:close()
    return strtrim(r)
end

function mt:crtpath(platform)
    return self.__path / 'VC' / 'Redist' / 'MSVC' / self:redistversion() / platform / ('Microsoft.VC' .. self.version .. '.CRT')
end

function mt:sdkpath()
    local reg = registry.open [[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows Kits\Installed Roots]]
    return fs.path(reg.KitsRoot10)
end

function mt:ucrtpath(platform)
    return self:sdkpath() / 'Redist' / 'ucrt' / 'DLLs' / platform
end

function mt:copy_crt_dll(platform, target)
    fs.create_directories(target)
    fs.copy_file(self:crtpath(platform) / 'msvcp140.dll', target / 'msvcp140.dll', true)
    fs.copy_file(self:crtpath(platform) / 'vcruntime140.dll', target / 'vcruntime140.dll', true)
    for dll in self:ucrtpath(platform):list_directory() do
        fs.copy_file(dll, target / dll:filename(), true)
    end
end

function mt:compile(target, solution, property)
    property.Configuration = property.Configuration or 'Release'
    property.Platform = property.Platform or 'Win32'
    local pstr = {}
    for k, v in pairs(property) do
        pstr[#pstr+1] = ('%s="%s"'):format(k, v)
    end
    local command = ('MSBuild "%s" /m /v:m /t:%s /clp:ShowEventId /p:%s'):format(solution:string(), target, table.concat(pstr, ','))
    local f = io.popen(command, 'r')
    if self.coding == 'ansi' then
        for line in f:lines() do
            print(line)
        end
    else
        for line in f:lines() do
            print(uni.a2u(line))
        end
    end
    local ok = f:close()
    if not ok then
        if self.coding == 'ansi' then
            error(("execute failed: %q"):format(uni.u2a(command)))
        else
            error(("execute failed: %q"):format(command))
        end
    end
end

return mt
