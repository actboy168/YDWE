/*
 * Hooking functions
 */
#ifndef YDWE_HOOK_H_INCLUDED
#define YDWE_HOOK_H_INCLUDED

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include <string>
#include <windows.h>

namespace NYDWE
{
/** \brief Initialize hooks
 *
 * Initialize and install hooks 
 */
void InstallHooks();

/** \brief Cleanup hooks
 *
 * Uninstall and cleanup hooks
 */
void UninstallHooks();

}

#endif // YDWE_HOOK_H_INCLUDED
