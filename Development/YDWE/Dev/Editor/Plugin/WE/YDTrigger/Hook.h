#ifndef INCLUDE_HOOK_H
#define INCLUDE_HOOK_H

#include <windows.h>

typedef struct tagAPIInfo
{
    PBYTE api_addr;
    BYTE  save_bytes[5];
    PVOID hook_func;
    BYTE  hook_mode;
}
APIInfo, *PAPIInfo;

BOOL HookOne(PAPIInfo pinfo);
BOOL UnhookOne(PAPIInfo pinfo);

#endif
