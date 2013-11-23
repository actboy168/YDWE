require "mapanalyzer"
require "filesystem"
require "util"
require "localization"
require "interface_storm"

local _westrings = {}

function _westrings.initialize ()
	local obj = {}
	obj.table_ = mapanalyzer.ini_table()
	obj.interface = interface_stormlib()
	obj.interface:open_path(fs.ydwe_path() / "share" / "mpq" / "ydwe")
	obj.manager = mapanalyzer.manager2(obj.interface)
	obj.manager:load_file('ui\\worldeditstrings.txt', obj.table_)
	obj.object_ = obj.table_:get('WorldEditStrings') or mapanalyzer.ini_object()
	return obj
end

function _westrings._index (self, key)
	return _(self.object_:get(key)) or ''
end

function _westrings.create (self)
	return setmetatable(self.initialize(), { __index = self._index })
end

westrings = _westrings:create()
