fs = require 'bee.filesystem'
local stormlib = require 'ffi.stormlib'

local jasshelper = {}

jasshelper.path = fs.ydwe_path() / "compiler" / "jasshelper"

local config = [[
[jasscompiler]
%q
"%s$COMMONJ $BLIZZARDJ $WAR3MAPJ"
]]

function jasshelper:prepare_common_j(map_path, version)
    if fs.exists(map_path / 'common.j') then
        return map_path / 'common.j'
    elseif fs.exists(map_path / 'scripts' / 'common.j') then
        return map_path / 'scripts' / 'common.j'
    else
        return fs.ydwe_devpath() / "compiler" / "jass" / tostring(version) / "common.j"
    end
end

function jasshelper:prepare_blizzard_j(map_path, version)
    if fs.exists(map_path / 'blizzard.j') then
        return map_path / 'blizzard.j'
    elseif fs.exists(map_path / 'scripts' / 'blizzard.j') then
        return map_path / 'scripts' / 'blizzard.j'
    else
        return fs.ydwe_devpath() / "compiler" / "jass" / tostring(version) / "blizzard.j"
    end
end

function jasshelper:createConfig(op)
	if op.option.pjass == '1' then
		io.save(fs.ydwe_path() / 'jasshelper.conf', config:format('../pjass/pjass-classic.exe', ''))
	else
		if op.option.runtime_version == 24 then
			io.save(fs.ydwe_path() / 'jasshelper.conf', config:format('../pjass/pjass-latest.exe', ''))
		else
			io.save(fs.ydwe_path() / 'jasshelper.conf', config:format('../pjass/pjass-latest.exe', '+rb '))
		end
	end
end

function jasshelper:compile(op)	
	log.trace("JassHelper compilation start.")
	self:createConfig(op)
    local common_j_path = self:prepare_common_j(op.map_path, op.option.runtime_version)
    local blizzard_j_path = self:prepare_blizzard_j(op.map_path, op.option.runtime_version)
    
	local parameter = {}
	
	-- 需要做vJass编译？
	if op.option.enable_jasshelper then
		-- debug选项（--debug）
		if op.option.enable_jasshelper_debug then
			parameter[#parameter + 1] = "--debug"
		end
		-- （关闭）优化选项（--nooptimize）
		if not op.option.enable_jasshelper_optimization then
			parameter[#parameter + 1] = "--nooptimize"
		end
	else
		-- 不编译vJass选项（--nopreprocessor）
		parameter[#parameter + 1] = "--nopreprocessor"
	end

    local ok = not not sys.spawn_wait {
        (self.path / "jasshelper.exe"):string(),
        parameter,
        "--scriptonly",
        common_j_path:string(),
        blizzard_j_path:string(),
        op.input:string(),
        op.output:string(),
        cwd = fs.ydwe_path():string(),
    }
    fs.remove(fs.ydwe_path() / 'jasshelper.conf')
    return ok
end

return jasshelper
