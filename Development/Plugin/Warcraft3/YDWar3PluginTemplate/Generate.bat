@echo off


set /p ProjectName=请输入工程名:

cd ..
if exist %ProjectName% rd /s /q %ProjectName%
md %ProjectName%

cd %ProjectName%

..\..\..\..\..\OpenSource\PHP\5.4.0\php.exe ..\YDWar3PluginTemplate\DllMain.cpp %ProjectName% > DllMain.cpp
..\..\..\..\..\OpenSource\PHP\5.4.0\php.exe ..\YDWar3PluginTemplate\Template.def %ProjectName% > %ProjectName%.def
..\..\..\..\..\OpenSource\PHP\5.4.0\php.exe ..\YDWar3PluginTemplate\Template.filters %ProjectName% > %ProjectName%.vcxproj.filters
..\..\..\..\..\OpenSource\PHP\5.4.0\php.exe ..\YDWar3PluginTemplate\Template.vcxproj %ProjectName% > %ProjectName%.vcxproj
..\..\..\..\..\OpenSource\PHP\5.4.0\php.exe ..\YDWar3PluginTemplate\version.rc %ProjectName% > version.rc

pause
