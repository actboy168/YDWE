@echo off
%1
cd %2 >nul
cls
echo Welcome to the Miles Sound System!
echo.
echo First, we'll demo the digital playback examples.
pause
cls
if exist digplay.exe digplay ..\media\glass.wav
pause
cls
if exist stream.exe streamer ..\media\wrong.mp3
pause
cls
if exist sstest.exe sstest
pause
cls

echo Now, we'll try the MIDI examples.
pause
cls
if exist dlsplay.exe dlsplay ..\media\demo.xmi
pause
cls
if exist xmiplay.exe xmiplay ..\media\demo.xmi
pause
cls

echo Now, we'll try the quick API examples.
pause
cls
if exist play.exe play ..\media\wrong.mp3
pause
cls
if exist play.exe play ..\media\demo.xmi
cls

echo Now, we'll try the record example.
pause
cls
if exist record.exe record 
cls

echo Finally, we'll try the redbook CD audio example.
pause
cls
if exist audiocd.exe audiocd
cls
echo All done!

