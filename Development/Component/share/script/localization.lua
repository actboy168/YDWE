require "i18n"
require "util"

local cm = i18n.conv_method((2 << 16) | string.byte("?", 1))


function _(str)
	return i18n.gettext(str)
end

function __(str)
	return i18n.utf8_to_ansi(str, cm)
end

function utf8_to_ansi(str)
	return i18n.utf8_to_ansi(str, cm)
end

function ansi_to_utf8(str)
	return i18n.ansi_to_utf8(str, cm)
end

i18n.textdomain("MainScript");
i18n.bindtextdomain("MainScript", fs.ydwe_path() / "share" / "locale")
