require "i18n"
require "filesystem"
local uni = require 'ffi.unicode'

function _(str)
	return i18n.get_text(str)
end

local ydwe = fs.ydwe_path():parent_path():remove_filename():remove_filename() / "Component"
if not fs.exists(ydwe) then
	ydwe = fs.ydwe_path()
end

i18n.initialize(fs.ydwe_devpath() / "share" / "locale")
i18n.set_domain("script")
