require "util"

local _file = fs.ydwe_path() / "bin" / "EverConfig.cfg"
local _object

function global_config_reload()
	_object = sys.ini_load(_file:string())
end
global_config_reload()

global_config = _object
