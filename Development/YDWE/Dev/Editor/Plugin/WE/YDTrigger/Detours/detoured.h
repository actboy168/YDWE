//////////////////////////////////////////////////////////////////////////////
//
//  Presence of this DLL (detoured.dll) marks a process as detoured.
//
//  Microsoft Research Detours Package, Version 2.1.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

HMODULE WINAPI Detoured();

BOOL WINAPI Detoured_Init(HINSTANCE hinst);


BOOL Detoured_Hook(PVOID *ppPointer, PVOID pDetour);
BOOL Detoured_Unhook(PVOID *ppPointer, PVOID pDetour);

#ifdef __cplusplus
}
#endif

//
///////////////////////////////////////////////////////////////// End of File.