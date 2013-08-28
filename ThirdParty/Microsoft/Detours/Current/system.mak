##############################################################################
##
##  Establish build target type for Detours.
##
##  Microsoft Research Detours Package, Version 3.0.
##
##  Copyright (c) Microsoft Corporation.  All rights reserved.
##

############################################## Determine Processor Build Type.
##
!IF "$(DETOURS_TARGET_PROCESSOR)" == "" && "$(PROCESSOR_ARCHITEW6432)" != ""
DETOURS_TARGET_PROCESSOR = X86
!ENDIF

!IF "$(DETOURS_TARGET_PROCESSOR)" == ""
DETOURS_TARGET_PROCESSOR = $(PROCESSOR_ARCHITECTURE)
!ENDIF

!IF "$(DETOURS_TARGET_PROCESSOR)" == "x86"
DETOURS_TARGET_PROCESSOR = X86
!ENDIF

!IF "$(DETOURS_TARGET_PROCESSOR)" == "AMD64"
DETOURS_TARGET_PROCESSOR = X64
!ENDIF

!IF "$(DETOURS_TARGET_PROCESSOR)" == "ARM"
DETOURS_TARGET_PROCESSOR = ARM
!ENDIF

########################################## Configure build based on Processor.
##
## DETOURS_OPTION_PROCESSOR: Set this macro if the processor *will* run code
##                           from another ISA (i.e. x86 on x64).
##
##      DETOURS_OPTION_BITS: Set this macro if the processor *may* have an
##                           an alternative word size.
##
!IF "$(DETOURS_TARGET_PROCESSOR)" == "X64"
#!MESSAGE Building for 64-bit X64.
DETOURS_SOURCE_BROWSING = 0
DETOURS_64BIT=1
DETOURS_BITS=64
DETOURS_OPTION_PROCESSOR=X86
DETOURS_OPTION_BITS=32
!ELSEIF "$(DETOURS_TARGET_PROCESSOR)" == "IA64"
#!MESSAGE Building for 64-bit IA64.
DETOURS_64BIT=1
DETOURS_BITS=64
DETOURS_OPTION_PROCESSOR=X86
DETOURS_OPTION_BITS=32
!ELSEIF "$(DETOURS_TARGET_PROCESSOR)" == "X86"
#!MESSAGE Building for 32-bit X86.
DETOURS_32BIT=1
DETOURS_BITS=32
DETOURS_OPTION_BITS=64
# Don't set DETOURS_OPTION_PROCESSOR for x64 because we don't *know* that
# we'll run on a 64-bit machine.
!ELSEIF "$(DETOURS_TARGET_PROCESSOR)" == "ARM"
#!MESSAGE Building for 32-bit ARM.
DETOURS_32BIT=1
DETOURS_BITS=32
!ELSE
!MESSAGE Note: To select the target processor architecture set either
!MESSAGE       PROCESSOR_ARCHITECTURE or DETOURS_TARGET_PROCESSOR.
!MESSAGE
!ERROR Unknown target processor: $(DETOURS_TARGET_ARCHITECTURE)
!ENDIF

##############################################################################
##
INCD = $(ROOT)\include
LIBD = $(ROOT)\lib.$(DETOURS_TARGET_PROCESSOR)
BIND = $(ROOT)\bin.$(DETOURS_TARGET_PROCESSOR)
OBJD = obj.$(DETOURS_TARGET_PROCESSOR)
!IF "$(DETOURS_OPTION_PROCESSOR)" != ""
OPTD = $(ROOT)\bin.$(DETOURS_OPTION_PROCESSOR)
!ENDIF

INCDS = $(ROOT)\include
LIBDS = $(ROOT)\lib.x86 $(ROOT)\lib.x64 $(ROOT)\lib.ia64 $(ROOT)\lib.arm
BINDS = $(ROOT)\bin.x86 $(ROOT)\bin.x64 $(ROOT)\bin.ia64 $(ROOT)\bin.arm
OBJDS = obj.x86 obj.x64 obj.ia64 obj.arm

##############################################################################

