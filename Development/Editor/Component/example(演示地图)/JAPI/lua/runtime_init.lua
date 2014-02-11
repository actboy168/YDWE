-- 开启一个调试窗口，用于显示调试信息，正式的版本请去掉。
-- 注：若在本文件内有语法错误，这行代码就不会被执行，语法错误也无法显示。所以
--    需要在一切都开始前，执行它；并确保本文件内没有错误。
jass_ext.runtime.console = true

-- 注册错误回调函数，当执行lua发生错误时，会调用此函数。
-- 注：注册此函数会略微增加运行时消耗（即使没有发生错误）。
function jass_ext.runtime.error_handle(msg)
	print("---------------------------------------")
	print("               LUA ERROR               ")
	print("---------------------------------------")
	print(tostring(msg) .. "\n")
	print(debug.traceback())
	print("---------------------------------------")
end

--
-- 设定调用cj函数返回的handle的type。(默认值为1)
-- 安全性依次递增，同时效率也会依次下降。
-- 0： 返回number。
---- 没有任何额外的安全性控制。
--
-- 1： 返回lightuserdata。
---- 只能判断是否相等，无法比较大小，并且不能隐式转换为整数。这和jass内的行为保持一致。
---- 但支持tostring，可以用于debug。
--
-- 2： 返回userdata。
---- lua持有handle时会增加handle的引用计数，并在__gc元方法中释放handle。
---- 0不再能被隐式转换为nil，对应jass里的null请使用nil。
--
jass_ext.runtime.handle_level = 1
