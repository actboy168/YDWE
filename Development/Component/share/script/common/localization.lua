require "i18n"
require "filesystem"
local uni = require 'ffi.unicode'

function _(str)
	return i18n.get_text(str)
end

i18n.initialize(fs.ydwe_devpath() / "share" / "locale")
i18n.set_domain("script")
