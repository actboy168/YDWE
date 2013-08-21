--------------------------------------------------------------------------
-- YD编辑器的主配置和代码文件
-- 本文件编码为UTF-8，在处理字符串时要注意使用iconv库转换
--------------------------------------------------------------------------

-- 特别说明：本文件内容被分拆为多个较为简单的子文件，以保证结构清晰，阅读方便


require "log"

log.debug("********************* ydwe initialize start *********************")

local real_require = require
require = function (str, ...)
	if not package.loaded[str] then
		log.trace("Initializing '" .. str .. "'")
	end
	return real_require(str, ...)
end

require "sys"
require "filesystem"
require "gui"
require "util"
require "event"
require "ar_stormlib"
require "ar_sfmpq"
require "ar_storm"

require "registry"
require "version"
require "localization"
require "config"
require "plugin"

require "ydwe_on_menu"
require "ydwe_on_save"
require "ydwe_on_startup"
require "ydwe_on_dialog_message"
require "ydwe_on_new_object_id"

log.debug("********************* ydwe initialize end *********************")
