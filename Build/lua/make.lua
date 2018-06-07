local configuration = arg[1] or 'Debug'
local dev = arg[3] ~= nil

if type(arg[2]) == 'string' then
    local out = io.open(arg[2], 'w')
    if out then
        local oldprint = print
        function print(...)
            out:write(table.concat(table.pack(...), '\t') .. '\n')
            oldprint(...)
        end
    end
end

require 'filesystem'
local filelock = require 'filelock'

local oklock = filelock('ok.lock')

oklock:unlock()

-- Step.1 初始化
local msvc = require 'msvc'
if not msvc:initialize(141) then
    error('Cannot found Visual Studio Toolset.')
end

local root = require 'path'
if not root then
    error('Not found `build.root`.')
end

local path = {}
path.Root = fs.path(root)
path.Build = path.Root / 'Build'
path.OpenSource = path.Root / 'OpenSource'
path.Development = path.Root / 'Development'
path.Result = path.Development / 'Build' / 'bin' / configuration

-- Step.2 清理
fs.remove_all(path.Development / 'Build' / 'bin' / configuration)
fs.remove_all(path.Development / 'Build' / 'obj' / configuration)
fs.remove_all(path.Development / 'Build' / 'lib' / configuration)
fs.remove_all(path.Build / 'publish' / configuration)

-- Step.3 版本信息
local function split(str, p)
	local rt = {}
	str:gsub('[^'..p..']+', function (w) table.insert(rt, w) end)
	return rt
end

local function version()
	local f = assert(io.open((path.Development / 'Core' / 'YDWEVersion' / 'YDWEVersion.h'):string(), 'r'))
	local version = f:read 'a'
	f:close()
	local major = version:match('YDWE_VERSION_MAJOR +(%d+)')
	local minor = version:match('YDWE_VERSION_MINOR +(%d+)')
	local revised = version:match('YDWE_VERSION_REVISED +(%d+)')
	return major, minor, revised
end

local function version_build()
	fs.remove(path.Development / 'Build' / 'include' / 'YDWEVersion.h')
	fs.remove(path.Development / 'Build' / 'include' / 'YDWEVersionBuild.h')
	if fs.exists(path.Build / 'include'/ 'version') then
		fs.create_directories(path.Development / 'Build' / 'include')
		local f = assert(io.open((path.Build / 'include'/ 'version'):string(), 'r'))
		local build = f:read 'a'
		f:close()
		local build = tonumber(build) or 0
		local f = assert(io.open((path.Development / 'Build' / 'include' / 'YDWEVersionBuild.h'):string(), 'w'))
		f:write(([[
	#ifndef YDWE_VERSION_BUILD_H_INCLUDED
	#define YDWE_VERSION_BUILD_H_INCLUDED

	#define YDWE_VERSION_BUILD %d

	#endif // YDWE_VERSION_BUILD_H_INCLUDED
		]]):format(build))
		f:close()
		return build
	end
	return 0
end

local major, minor, revised = version()
local build = version_build()
print(('build %d.%d.%d.%d'):format(major, minor, revised, build))

local gitlog = require 'gitlog'
gitlog((path.Development / 'Component' / 'script' / 'common' / 'gitlog.lua'):string())

-- Step.4 编译
local property = {
    Configuration = configuration,
    Platform = 'Win32'
}
msvc:compile('rebuild', path.OpenSource / 'all.sln', property)
if not dev then
    msvc:compile('rebuild', path.Development / 'Core' / 'Solution' / 'YDWE.sln', property)
end

-- Step.5 复制
local function copy_directory(from, to, filter)
    fs.create_directories(to)
	for fromfile in from:list_directory() do
		if fs.is_directory(fromfile) then
			copy_directory(fromfile, to / fromfile:filename(), filter)
		else
			if (not filter) or filter(fromfile) then
                fs.copy_file(fromfile, to / fromfile:filename(), true)
            end
		end
	end
end

fs.create_directories(path.Result / 'bin' / 'modules')
fs.create_directories(path.Result / 'compiler' / 'jasshelper' / 'bin')
if configuration == 'Release' then
    msvc:copy_crt_dll('x86', path.Result / 'bin')
end
fs.copy_file(path.OpenSource / 'Lua' / 'build' / 'bin' / configuration / 'lua53.dll', path.Result / 'bin' / 'lua53.dll', true)
fs.copy_file(path.OpenSource / 'Lua' / 'build' / 'bin' / configuration / 'lua.exe', path.Result / 'bin' / 'lua.exe', true)
fs.copy_file(path.OpenSource / 'StormLib' / 'bin' / 'Win32' / configuration / 'StormLib.dll', path.Result / 'bin' / 'StormLib.dll', true)
fs.copy_file(path.OpenSource / 'minizip' / 'bin' / configuration / 'minizip.dll', path.Result / 'bin' / 'minizip.dll', true)
fs.copy_file(path.OpenSource / 'luaffi' / 'bin' / configuration / 'ffi.dll', path.Result / 'bin' / 'modules' / 'ffi.dll', true)
fs.copy_file(path.OpenSource / 'lpeg' / 'bin' / configuration / 'lpeg.dll', path.Result / 'bin' / 'modules' / 'lpeg.dll', true)
fs.copy_file(path.OpenSource / 'sfmpq' / 'bin' / configuration / 'sfmpq.dll', path.Result / 'compiler' / 'jasshelper' / 'sfmpq.dll', true)
fs.copy_file(path.OpenSource / 'sfmpq' / 'bin' / configuration / 'sfmpq.dll', path.Result / 'compiler' / 'jasshelper' / 'bin' / 'sfmpq.dll', true)
copy_directory(path.Development / 'Component', path.Result)

-- Step.7 复制到publish
local ignore = { ['.pdb'] = true, ['.exp'] = true, ['.ilk'] = true, ['.aps'] = true, ['.lib'] = true, ['.ipdb'] = true, ['.iobj'] = true }
copy_directory(path.Result, path.Build / 'publish' / configuration, 
    function(path)
        local ext = path:extension():string():lower()
        return not ignore[ext]
    end
)

oklock:lock()

local time = math.floor(os.clock())
print(('time: %d:%02d'):format(time//60, time%60))
