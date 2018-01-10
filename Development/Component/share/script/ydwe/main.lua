--------------------------------------------------------------------------
-- YD编辑器的主配置和代码文件
-- 本文件编码为UTF-8，在处理字符串时要注意使用iconv库转换
--------------------------------------------------------------------------

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
require "filesystem"
require "gui"

local uni = require "ffi.unicode"
local root = fs.ydwe_devpath()
package.path = package.path.. ';' .. uni.u2a((root / 'plugin' / '?.lua'):string()) .. ';' .. uni.u2a((root / 'plugin' / '?' / 'init.lua'):string()) 

require "event"
require "virtual_mpq"
require "registry"
require "version"
require "localization"
require "config"
require "plugin"
require "check_object"
require "ffi.loadlibrary"
require "ffi.gui"

require "ydwe_on_menu"
require "ydwe_on_save"
require "ydwe_on_test"
require "ydwe_on_startup"
require "ydwe_on_dialog_message"
require "ydwe_on_new_object_id"

log.debug("********************* ydwe initialize end *********************")
