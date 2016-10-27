@echo OFF
CD lua\luabuild\bin
".\lua.exe" "..\..\make.lua" "%~dp0lua" %1 "%~dp0build.log" %2
CD ..\..\..
