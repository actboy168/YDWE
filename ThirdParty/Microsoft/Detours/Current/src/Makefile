##############################################################################
##
##  Makefile for Detours.
##
##  Microsoft Research Detours Package, Version 3.0 Build_316.
##
##  Copyright (c) Microsoft Corporation.  All rights reserved.
##

ROOT = ..
!include "$(ROOT)\system.mak"

!IF "$(DETOURS_SOURCE_BROWSING)" == ""
DETOURS_SOURCE_BROWSING = 0
!ENDIF

#######################/#######################################################
##
CFLAGS=/W4 /WX /Zi /MTd /Gy /Gm- /Zl /Od /DDETOURS_BITS=$(DETOURS_BITS)
!IF $(DETOURS_SOURCE_BROWSING)==1
CFLAGS=$(CFLAGS) /FR
!ELSE
CFLAGS=$(CFLAGS) /DWIN32_LEAN_AND_MEAN /D_WIN32_WINNT=0x403
!ENDIF

#/D_CRT_INSECURE_DEPRECATE

LIBFLAGS=/nologo
LINKFLAGS=/release

!IF "$(DETOURS_TARGET_PROCESSOR)" == "X86"

CFLAGS=$(CFLAGS) /Gs /DDETOURS_X86=1 /DDETOURS_32BIT=1 /D_X86_
LIBFLAGS=$(LIBFLAGS)
DLLENTRY=_DllMainCRTStartup@12
LINKFLAGS=$(LINKFLAGS) /machine:x86

!ELSEIF "$(DETOURS_TARGET_PROCESSOR)" == "X64"

CFLAGS=$(CFLAGS) /D_WIN64 /DDETOURS_X64=1 /DDETOURS_64BIT=1 /D_AMD64_
LIBFLAGS=$(LIBFLAGS)
DLLENTRY=_DllMainCRTStartup
LINKFLAGS=$(LINKFLAGS) /machine:amd64

!ELSEIF "$(DETOURS_TARGET_PROCESSOR)" == "IA64"

CFLAGS=$(CFLAGS) /D_WIN64 /DDETOURS_IA64=1 /DDETOURS_64BIT=1 /D_IA64_ /wd4163
LIBFLAGS=$(LIBFLAGS)
DLLENTRY=_DllMainCRTStartup
LINKFLAGS=$(LINKFLAGS) /machine:ia64

!ELSEIF "$(DETOURS_TARGET_PROCESSOR)" == "ARM"

CFLAGS=$(CFLAGS) /Gs /DDETOURS_ARM=1 /DDETOURS_32BIT=1 /D_ARM_ /D_ARM_WORKAROUND_
LIBFLAGS=$(LIBFLAGS)
DLLENTRY=_DllMainCRTStartup@12
LINKFLAGS=$(LINKFLAGS) /machine:arm

!ENDIF

!IF "$(DETOURS_OPTION_BITS)" != ""
CFLAGS=$(CFLAGS) /DDETOURS_OPTION_BITS=$(DETOURS_OPTION_BITS)
!ENDIF

OBJS = \
    $(OBJD)\detours.obj     \
    $(OBJD)\modules.obj     \
    $(OBJD)\disasm.obj      \
    $(OBJD)\image.obj       \
    $(OBJD)\creatwth.obj    \

##############################################################################
##
.SUFFIXES: .cpp .h .obj

.cpp{$(OBJD)}.obj:
    $(CC) $(CFLAGS) /Fd$(LIBD)\detours.pdb /Fo$@ /c .\$(*B).cpp

##############################################################################

all: dirs \
    $(LIBD)\detours.lib \
    $(INCD)\detours.h   \
    $(INCD)\detver.h    \
!IF $(DETOURS_SOURCE_BROWSING)==1
    $(OBJD)\detours.bsc \
!endif

##############################################################################

clean:
    @-del *~ 2>nul
    @-del $(LIBD)\detours.pdb $(LIBD)\detours.lib 2>nul
    @-rmdir /q /s $(OBJD) 2>nul

realclean: clean
    @-rmdir /q /s $(OBJDS) 2>nul

##############################################################################

dirs:
    @if not exist "$(INCD)" mkdir "$(INCD)" && echo.   Created $(INCD)
    @if not exist "$(LIBD)" mkdir "$(LIBD)" && echo.   Created $(LIBD)
    @if not exist "$(BIND)" mkdir "$(BIND)" && echo.   Created $(BIND)
    @if not exist "$(OBJD)" mkdir "$(OBJD)" && echo.   Created $(OBJD)

$(OBJD)\detours.bsc : $(OBJS)
    bscmake /v /n /o $@ $(OBJS:.obj=.sbr)

$(LIBD)\detours.lib : $(OBJS)
    lib $(LIBFLAGS) /out:$@ $(OBJS)

$(INCD)\detours.h : detours.h
    copy detours.h $@

$(INCD)\detver.h : detver.h
    copy detver.h $@

$(OBJD)\detours.obj : detours.cpp detours.h
$(OBJD)\modules.obj : modules.cpp detours.h
$(OBJD)\disasm.obj : disasm.cpp detours.h
$(OBJD)\image.obj : image.cpp detours.h
$(OBJD)\creatwth.obj : creatwth.cpp uimports.cpp detours.h

test: all
    cd $(MAKEDIR)\..\samples\slept
    nmake /nologo test
    cd $(MAKEDIR)

################################################################# End of File.
