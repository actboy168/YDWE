local i18n = require "i18n"

i18n.initialize(fs.ydwe_devpath() / "share" / "locale")
i18n.set_domain("script")

log.error('[i18n]current language', i18n.get_language())

LNG = setmetatable({}, { __index = function (_, str)
    local res = i18n.get_text(str)
    if res == str then
        log.error('[i18n]unknown text:', str)
    end
    return res
end})
