@echo OFF
CD lua\luabuild\bin
".\lua.exe" "..\..\make.lua" "%~dp0lua" %1 %2 %3
CD ..\..\..
