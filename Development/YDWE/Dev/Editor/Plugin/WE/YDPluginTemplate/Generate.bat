@echo off


set /p ProjectName=请输入工程名:

cd ..
if exist %ProjectName% rd /s /q %ProjectName%
md %ProjectName%

cd %ProjectName%

..\..\..\..\..\..\OpenSource\PHP\5.4.0\php.exe ..\YDPluginTemplate\DllMain.cpp %ProjectName% > DllMain.cpp
..\..\..\..\..\..\OpenSource\PHP\5.4.0\php.exe ..\YDPluginTemplate\Template.def %ProjectName% > %ProjectName%.def
..\..\..\..\..\..\OpenSource\PHP\5.4.0\php.exe ..\YDPluginTemplate\Template.filters %ProjectName% > %ProjectName%.vcxproj.filters
..\..\..\..\..\..\OpenSource\PHP\5.4.0\php.exe ..\YDPluginTemplate\Template.vcxproj %ProjectName% > %ProjectName%.vcxproj

pause
