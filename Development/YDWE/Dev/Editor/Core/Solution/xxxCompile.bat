@call "%VS100COMNTOOLS%\vsvars32.bat"

@echo "正在重新构建YDUI：YDWE|Win32"
devenv ../../UI/sln/YDUI.sln /Rebuild "YDWE|Win32"

@echo "正在重新构建YDUI：YDTrigger|Win32"
devenv ../../UI/sln/YDUI.sln /Rebuild "YDTrigger|Win32"

@echo "拷贝UI到bin目录"
xcopy ..\..\UI\bin ..\..\Build\bin\Release\share\mpq\ /d /y /e

