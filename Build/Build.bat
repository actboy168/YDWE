@echo OFF
CD lua\luabuild\bin
"%~dp0lua\luabuild\bin\lua.exe" "%~dp0lua\make.lua" "%~dp0lua" %1 "%~dp0build.log" %2
CD ..\..\..
