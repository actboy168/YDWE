@echo off
if exist "%~dp0ok.lock" (set errorlevel=0) else (set errorlevel=1)
echo errorlevel: %errorlevel%
exit /b %errorlevel%