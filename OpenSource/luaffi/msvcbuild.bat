@call "%VS100COMNTOOLS%\vsvars32.bat"

@if not defined INCLUDE goto :FAIL
@if "%1"=="clean" goto :CLEAN

@setlocal

@set LUA_INCLUDE=..\Lua\Current\include
@set LUA_LIB=..\Lua\Current\lib\Win32\Release\luacore.lib
@set LUA_EXE=..\Lua\Current\bin\Win32\Release\lua.exe
@set LUA_DLL=luacore.dll

@set DO_CL=cl.exe /nologo /c /MDd /FC /Zi /Od /W3 /WX /D_CRT_SECURE_NO_DEPRECATE /DLUA_FFI_BUILD_AS_DLL /I"msvc"
@set DO_LINK=link /nologo /debug
@set DO_MT=mt /nologo

@if "%1"=="debug" goto :COMPILE
@if "%1"=="release" goto :RELEASE

:RELEASE
@set DO_CL=cl.exe /nologo /c /MD /Ox /W3 /Zi /WX /D_CRT_SECURE_NO_DEPRECATE /DLUA_FFI_BUILD_AS_DLL /I"msvc"
@set DO_LINK=link.exe /nologo /debug
@set DO_MT=mt.exe /nologo

:COMPILE
"%LUA_EXE%" dynasm\dynasm.lua -LNE -D X32WIN -o call_x86.h call_x86.dasc
"%LUA_EXE%" dynasm\dynasm.lua -LNE -D X64 -o call_x64.h call_x86.dasc
"%LUA_EXE%" dynasm\dynasm.lua -LNE -D X64 -D X64WIN -o call_x64win.h call_x86.dasc
"%LUA_EXE%" dynasm\dynasm.lua -LNE -o call_arm.h call_arm.dasc
%DO_CL% /I"." /I"%LUA_INCLUDE%" /DLUA_DLL_NAME="%LUA_DLL%" call.c ctype.c ffi.c parser.c
%DO_LINK% /DLL /OUT:ffi.dll "%LUA_LIB%" *.obj
if exist ffi.dll.manifest^
    %DO_MT% -manifest ffi.dll.manifest -outputresource:"ffi.dll;2"

@goto :CLEAN_OBJ

:CLEAN
del *.dll
:CLEAN_OBJ
del *.obj *.manifest
@goto :END

:FAIL
@echo You must open a "Visual Studio .NET Command Prompt" to run this script
:END

