require "i18n"
require "util"
local uni = require 'ffi.unicode'

function _(str)
	return i18n.gettext(str)
end

function __(str)
	return uni.u2a(str)
end

i18n.textdomain("MainScript");
i18n.bindtextdomain("MainScript", fs.ydwe_path() / "share" / "locale")
