require "i18n"
require "util"

local cm = i18n.conv_method((2 << 16) | string.byte("?", 1))

-- 转换为本地语言
-- str - UTF-8编码的文本ID
-- 返回：本地语言、本地编码（操作系统相关）的字符串
-- 注意：1. 找不到翻译信息的则保留英文
--       2. 本文件编码为UTF-8，因此所有硬编码的字符串均为UTF-8编码，要注意转换
function _(str)
	return i18n.utf8_to_ansi(i18n.gettext(str), cm)
end

-- 转换为本地编码
-- str - UTF-8编码的文本
-- 返回：本地编码（操作系统相关）的字符串
-- 注意：本文件编码为UTF-8，因此所有硬编码的字符串均为UTF-8编码，要注意转换
function __(str)
	return i18n.utf8_to_ansi(str, cm)
end

i18n.textdomain("MainScript");
i18n.bindtextdomain("MainScript", fs.ydwe_path() / "share" / "locale")
