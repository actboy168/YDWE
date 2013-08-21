//////////////////////////////////////////////////////////////////////////////
//
//  Presence of this DLL (detoured.dll) marks a process as detoured.
//
//  Microsoft Research Detours Package, Version 2.1.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include <windows.h>
#include "detoured.h"

static HMODULE s_hDll;

HMODULE WINAPI Detoured()
{
  return s_hDll;
}

BOOL WINAPI Detoured_Init(HINSTANCE hinst)
{
  s_hDll = hinst;
  DisableThreadLibraryCalls(hinst);
  return TRUE;
}
//
///////////////////////////////////////////////////////////////// End of File.


#include "detours.h"

BOOL Detoured_Hook(PVOID *ppPointer, PVOID pDetour)
{
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(ppPointer, pDetour);
  if (DetourTransactionCommit() != NO_ERROR)
    return FALSE;
  return TRUE;
}

BOOL Detoured_Unhook(PVOID *ppPointer, PVOID pDetour)
{
  if (*ppPointer == NULL)
    return FALSE;
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourDetach(ppPointer, pDetour);
  if (DetourTransactionCommit() != NO_ERROR)
    return FALSE;

  return TRUE;
}