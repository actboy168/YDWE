require "log"

log.debug("********************* ydwe initialize start *********************")

do
	local real_require = require
	require = function (str, ...)
		if not package.loaded[str] then
			log.trace("Initializing '" .. str .. "'")
		end
		return real_require(str, ...)
	end
end
require "utiliy"

local function compiler_searcher(name)
    local filename
    if name == 'compiler' then
        filename = fs.ydwe_devpath() / 'compiler' / 'script' / 'init.lua'
    elseif name:sub(1, 9) == 'compiler.' then
        filename = fs.ydwe_devpath() / 'compiler' / 'script' / (name:sub(10):gsub('%.', '/') .. '.lua')
    else
        return ("\n\tno file '%s'"):format(name)
    end
    local f, err = io.open(filename, 'r')
    if f then
        local str = f:read 'a'
        f:close()
        f, err = load(str, '@' .. filename:string())
    end
    if not f then
        error(("error loading module '%s' from file '%s':\n\t%s"):format(name, filename:string(), err))
    end
    return f, filename:string()
end
table.insert(package.searchers, compiler_searcher)

require "filesystem"
require "event"
require "virtual_mpq"
require "version"
require "localization"
require "config"
require "plugin"
require "ffi.loadlibrary"
require "ffi.gui"

require "ydwe_on_menu"
require "ydwe_on_save"
require "ydwe_on_test"
require "ydwe_on_startup"
require "ydwe_on_dialog_message"
require "ydwe_on_new_object_id"

log.debug("********************* ydwe initialize end *********************")
