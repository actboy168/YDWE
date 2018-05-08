if exist c:\project\ydwe\build\lua\ok.lock (set errorlevel=0) else (set errorlevel=1)
echo errorlevel: %errorlevel%
exit /b %errorlevel%