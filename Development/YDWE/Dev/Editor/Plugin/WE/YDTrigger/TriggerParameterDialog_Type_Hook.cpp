#include <windows.h>
#include <detours.h>
#include "Detours/detoured.h"
#include "Storm/StormAdapter.h"
#include "Common.h"
#include "Hook.h"

APIInfo g_TPD_type_info1;
APIInfo g_TPD_type_info2;

DWORD g_nRet_TPD_type1 = 0;
DWORD g_nRet_TPD_type2 = 0;
DWORD g_nTPD_type_flag = 0;

int _stdcall TPD_Type_Check1(LPCSTR string1, LPCSTR string2, size_t size)
{
    if (0 == BLZSStrCmp(string1, "degree", size))
    {
        g_nTPD_type_flag = 1;
        return 0;
    }
    else if (0 == BLZSStrCmp(string1, "radian", size))
    {
        g_nTPD_type_flag = 2;
        return 0;
    }

    return BLZSStrCmp(string1, string2, size);
}

int _stdcall TPD_Type_Check2(LPCSTR string1, LPCSTR string2, size_t size)
{
    if (g_nTPD_type_flag != 0)
    {
        g_nTPD_type_flag = 0;
        return 0;        
    }

    return BLZSStrCmp(string1, string2, size);
}

void __declspec(naked) TPD_Type_Hook1()
{
    __asm
    {
        call TPD_Type_Check1
        jmp [g_nRet_TPD_type1]
    }
}

void __declspec(naked) TPD_Type_Hook2()
{
    __asm
    {
        call TPD_Type_Check2
        jmp [g_nRet_TPD_type2]
    }
}

typedef int (_fastcall* TPD_TypeProc)(DWORD nThis, DWORD nEdx, LPCSTR string1, DWORD nUnk1, DWORD nUnk2, DWORD nUnk3);
TPD_TypeProc TPD_Type;

int _fastcall TPD_Type_Hook(DWORD nThis, DWORD nEdx, LPCSTR string1, DWORD nUnk1, DWORD nUnk2, DWORD nUnk3)
{
    if (g_nTPD_type_flag == 1)
    {
        return TPD_Type(nThis, nEdx, "degree", nUnk1, nUnk2, nUnk3);
    }
    else if (g_nTPD_type_flag == 2)
    {
        return TPD_Type(nThis, nEdx, "radian", nUnk1, nUnk2, nUnk3);
    }

    return TPD_Type(nThis, nEdx, string1, nUnk1, nUnk2, nUnk3);
}

void TriggerParameterDialog_Type_Hook()
{
    g_TPD_type_info1.api_addr = (PBYTE)WE_ADDRESS(0x0068C391);
    g_TPD_type_info1.hook_func = TPD_Type_Hook1;
    g_TPD_type_info1.hook_mode = 0xE9;// jmp
    g_nRet_TPD_type1 = WE_ADDRESS(0x0068C396);
    HookOne(&g_TPD_type_info1);

    g_TPD_type_info2.api_addr = (PBYTE)WE_ADDRESS(0x0068778E);
    g_TPD_type_info2.hook_func = TPD_Type_Hook2;
    g_TPD_type_info2.hook_mode = 0xE9;// jmp
    g_nRet_TPD_type2 = WE_ADDRESS(0x00687793);
    HookOne(&g_TPD_type_info2);

    TPD_Type = (TPD_TypeProc) WE_ADDRESS(0x00687650);
    DetourAttach((PVOID*)&TPD_Type, TPD_Type_Hook);
}

void TriggerParameterDialog_Type_Unhook()
{
    UnhookOne(&g_TPD_type_info1);
    UnhookOne(&g_TPD_type_info2);

    DetourDetach((PVOID*)&TPD_Type, TPD_Type_Hook);
}
