@echo off

\masm32\bin\rc /v rsrc.rc
\masm32\bin\cvtres /machine:ix86 rsrc.res
\masm32\bin\ml /c /Cp /Gz /Ic:\masm32\include /coff /nologo AdicHelper.asm
if errorlevel 1 goto err
\masm32\bin\link /SUBSYSTEM:WINDOWS /LIBPATH:c:\masm32\lib /nologo AdicHelper.obj rsrc.res
if errorlevel 1 goto err

del rsrc.res
del AdicHelper.obj
del rsrc.obj

goto exit

:err

del rsrc.res
del mx.obj
del rsrc.obj

pause

:exit