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


local function crtpath(platform)
    local RedistVersion = (function ()
        local verfile = installpath() / 'VC' / 'Auxiliary' / 'Build' / 'Microsoft.VCRedistVersion.default.txt'
        local f = assert(io.open(verfile:string(), 'r'))
        local r = f:read 'a'
        f:close()
        return strtrim(r)
    end)()
    local ToolVersion = (function ()
        local verfile = installpath() / 'VC' / 'Auxiliary' / 'Build' / 'Microsoft.VCToolsVersion.default.txt'
        local f = assert(io.open(verfile:string(), 'r'))
        local r = f:read 'a'
        f:close()
        return strtrim(r)
    end)()
    local ToolsetVersion = (function ()
        local verfile = installpath() / 'VC' / 'Tools' / 'MSVC' / ToolVersion / 'include' / 'yvals_core.h'
        local f = assert(io.open(verfile:string(), 'r'))
        local r = f:read 'a'
        f:close()
        return r:match '#define%s+_MSVC_STL_VERSION%s+(%d+)'
    end)()
    return installpath() / 'VC' / 'Redist' / 'MSVC' / RedistVersion / platform / ('Microsoft.VC'..ToolsetVersion..'.CRT')
end

local function ucrtpath(platform)
    local registry = require 'bee.registry'
    local reg = registry.open [[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows Kits\Installed Roots]]
    local path = fs.path(reg.KitsRoot10) / 'Redist'
    local res, ver
    local function accept(p)
        if not fs.is_directory(p) then
            return
        end
        local ucrt = p / 'ucrt' / 'DLLs' / platform
        if fs.exists(ucrt) then
            local version = 0
            if p ~= path then
                version = p:filename():string():gsub('10%.0%.([0-9]+)%.0', '%1')
                version = tonumber(version)
            end
            if not ver or ver < version then
                res, ver = ucrt, version
            end
        end
    end
    accept(path)
    for p in path:list_directory() do
        accept(p)
    end
    if res then
        return res
    end
end

local function copy_crtdll(platform, target)
    fs.create_directories(target)
    for dll in crtpath(platform):list_directory() do
        if dll:filename() ~= fs.path "vccorlib140.dll" then
            fs.copy_file(dll, target / dll:filename(), true)
        end
    end
    for dll in ucrtpath(platform):list_directory() do
        fs.copy_file(dll, target / dll:filename(), true)
    end
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

function mt:copy_crt_dll(platform, target)
    copy_crtdll(platform, target)
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
