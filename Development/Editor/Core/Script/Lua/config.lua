require "util"

local _config = {}

function _config:get_integer(key_name, default_value)
	return self._object:get_integer(key_name, default_value)
end

function _config:get_string(key_name, default_value)
	return self._object:get_string(key_name, default_value)
end

function _config:reload()
	self._object:reload(self._file)
end

function _config:initialize()
	self._file   = fs.ydwe_path() / "bin" / "EverConfig.cfg"
	self._object = sys.config_property(self._file)
end

_config:initialize()

global_config = _config
