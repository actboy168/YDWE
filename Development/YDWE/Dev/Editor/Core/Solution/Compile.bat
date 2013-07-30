@call "%VS100COMNTOOLS%\vsvars32.bat"

@echo "正在重新构建YDWE：%1|Win32"
@echo "正在重新构建YDWE：%1|Win32" >> %2
devenv YDWE.sln /Rebuild "%1|Win32" /out %2

@echo "正在重新构建YDColorizer：%1|Any CPU"
@echo "正在重新构建YDColorizer：%1|Any CPU" >> %2
devenv ../../Plugin/YDColorizer/YDColorizer.sln /Rebuild "%1|Any CPU" /out %2

@echo "正在重新构建YDUI：YDWE|Win32"
@echo "正在重新构建YDUI：YDWE|Win32" >> %2
devenv ../../UI/sln/YDUI.sln /Rebuild "YDWE|Win32" /out %2

@echo "正在重新构建YDUI：YDTrigger|Win32"
@echo "正在重新构建YDUI：YDTrigger|Win32" >> %2
devenv ../../UI/sln/YDUI.sln /Rebuild "YDTrigger|Win32" /out %2

@echo "执行脚本"
@echo "执行脚本" >> %2
"..\..\..\..\..\..\OpenSource\Python\2.7.2\python.exe" "..\Script\Python\build_anti_bj_leak.py" %1
"..\..\..\..\..\..\OpenSource\Python\2.7.2\python.exe" "..\Script\Python\copy_all.py"  %1
"..\..\..\..\..\..\OpenSource\Python\2.7.2\python.exe" "..\Script\Python\pack_skin.py" %1
"..\..\..\..\..\..\OpenSource\Python\2.7.2\python.exe" "..\Script\Python\pack_ui.py" %1
"..\..\..\..\..\..\OpenSource\Python\2.7.2\python.exe" "..\Script\Python\pack_units.py" %1
