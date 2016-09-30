require "mapanalyzer"
require "filesystem"
require "interface_storm"

local mpq_path = fs.ydwe_path() / "share" / "mpq"
local interface = interface_stormlib()
interface:open_path(mpq_path / "units")
interface:open_path(mpq_path / "ydwe")
interface:attach_archive(__map_handle__)
local manager = mapanalyzer.new(interface)
return manager
