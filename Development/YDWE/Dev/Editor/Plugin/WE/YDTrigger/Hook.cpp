
#include "Hook.h"

BOOL HookOne(PAPIInfo pinfo)
{
    DWORD dwOldProtect;
    MEMORY_BASIC_INFORMATION mbi;

    if (pinfo->api_addr == NULL || pinfo->hook_func == NULL) return FALSE;

    VirtualQuery(pinfo->api_addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

    if (!VirtualProtect(mbi.BaseAddress,mbi.RegionSize,PAGE_READWRITE, &dwOldProtect)) return FALSE;

    //保存这5个字节，因为后面将覆盖此处
    memcpy(pinfo->save_bytes, pinfo->api_addr, sizeof(pinfo->save_bytes));

    pinfo->api_addr[0] = pinfo->hook_mode;
    *(DWORD*)&pinfo->api_addr[1] = (DWORD)pinfo->hook_func - (DWORD)pinfo->api_addr - 5;

    if (!VirtualProtect(mbi.BaseAddress,mbi.RegionSize, dwOldProtect, &dwOldProtect)) return FALSE;

    return TRUE;
}

BOOL UnhookOne(PAPIInfo pinfo)
{
    DWORD dwOldProtect;

    if (pinfo->api_addr == NULL || pinfo->hook_func == NULL) return FALSE;

    if (!VirtualProtect(pinfo->api_addr, 8, PAGE_READWRITE, &dwOldProtect)) return FALSE;

    WriteProcessMemory(GetCurrentProcess(), pinfo->api_addr, pinfo->save_bytes, sizeof(pinfo->save_bytes), NULL);

    if (!VirtualProtect(pinfo->api_addr, 8, dwOldProtect, NULL)) return FALSE;

    return TRUE;
}
