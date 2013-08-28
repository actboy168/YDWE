//////////////////////////////////////////////////////////////////////////////
//
//  Create a process with a DLL (creatwth.cpp of detours.lib)
//
//  Microsoft Research Detours Package, Version 3.0 Build_316.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include <windows.h>
#include <stddef.h>
#if (_MSC_VER < 1299)
typedef DWORD DWORD_PTR;
#endif
#if (_MSC_VER < 1310)
#else
#include <strsafe.h>
#endif

// #define DETOUR_DEBUG 1
// #define IGNORE_CHECKSUMS 1
#define DETOURS_INTERNAL

#include "detours.h"

#define IMPORT_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
#define BOUND_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT]
#define CLR_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR]
#define IAT_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT]

//////////////////////////////////////////////////////////////////////////////
//
#ifndef _STRSAFE_H_INCLUDED_
static inline HRESULT StringCchLengthA(const char* psz, size_t cchMax, size_t* pcch)
{
    HRESULT hr = S_OK;
    size_t cchMaxPrev = cchMax;

    if (cchMax > 2147483647)
    {
        return ERROR_INVALID_PARAMETER;
    }

    while (cchMax && (*psz != '\0'))
    {
        psz++;
        cchMax--;
    }

    if (cchMax == 0)
    {
        // the string is longer than cchMax
        hr = ERROR_INVALID_PARAMETER;
    }

    if (SUCCEEDED(hr) && pcch)
    {
        *pcch = cchMaxPrev - cchMax;
    }

    return hr;
}

static inline HRESULT StringCchCopyA(char* pszDest, size_t cchDest, const char* pszSrc)
{
    HRESULT hr = S_OK;

    if (cchDest == 0)
    {
        // can not null terminate a zero-byte dest buffer
        hr = ERROR_INVALID_PARAMETER;
    }
    else
    {
        while (cchDest && (*pszSrc != '\0'))
        {
            *pszDest++ = *pszSrc++;
            cchDest--;
        }

        if (cchDest == 0)
        {
            // we are going to truncate pszDest
            pszDest--;
            hr = ERROR_INVALID_PARAMETER;
        }

        *pszDest= '\0';
    }

    return hr;
}

static inline HRESULT StringCchCatA(char* pszDest, size_t cchDest, const char* pszSrc)
{
    HRESULT hr;
    size_t cchDestCurrent;

    if (cchDest > 2147483647)
    {
        return ERROR_INVALID_PARAMETER;
    }

    hr = StringCchLengthA(pszDest, cchDest, &cchDestCurrent);

    if (SUCCEEDED(hr))
    {
        hr = StringCchCopyA(pszDest + cchDestCurrent,
                            cchDest - cchDestCurrent,
                            pszSrc);
    }

    return hr;
}

#endif

//////////////////////////////////////////////////////////////////////////////
//
#if IGNORE_CHECKSUMS
static WORD detour_sum_minus(WORD wSum, WORD wMinus)
{
    wSum = (WORD)(wSum - ((wSum < wMinus) ? 1 : 0));
    wSum = (WORD)(wSum - wMinus);
    return wSum;
}

static WORD detour_sum_done(DWORD PartialSum)
{
    // Fold final carry into a single word result and return the resultant value.
    return (WORD)(((PartialSum >> 16) + PartialSum) & 0xffff);
}

static WORD detour_sum_data(DWORD dwSum, PBYTE pbData, DWORD cbData)
{
    while (cbData > 0) {
        dwSum += *((PWORD&)pbData)++;
        dwSum = (dwSum >> 16) + (dwSum & 0xffff);
        cbData -= sizeof(WORD);
    }
    return detour_sum_done(dwSum);
}

static WORD detour_sum_final(WORD wSum, PIMAGE_NT_HEADERS pinh)
{
    DETOUR_TRACE((".... : %08x (value: %08x)\n", wSum, pinh->OptionalHeader.CheckSum));

    // Subtract the two checksum words in the optional header from the computed.
    wSum = detour_sum_minus(wSum, ((PWORD)(&pinh->OptionalHeader.CheckSum))[0]);
    wSum = detour_sum_minus(wSum, ((PWORD)(&pinh->OptionalHeader.CheckSum))[1]);

    return wSum;
}

static WORD ChkSumRange(WORD wSum, HANDLE hProcess, PBYTE pbBeg, PBYTE pbEnd)
{
    BYTE rbPage[4096];

    while (pbBeg < pbEnd) {
        if (!ReadProcessMemory(hProcess, pbBeg, rbPage, sizeof(rbPage), NULL)) {
            DETOUR_TRACE(("ReadProcessMemory(chk@%p..%p) failed: %d\n",
                          pbBeg, pbEnd, GetLastError()));
            break;
        }
        wSum = detour_sum_data(wSum, rbPage, sizeof(rbPage));
        pbBeg += sizeof(rbPage);
    }
    return wSum;
}

static WORD ComputeChkSum(HANDLE hProcess, PBYTE pbModule, PIMAGE_NT_HEADERS pinh)
{
    // See LdrVerifyMappedImageMatchesChecksum.

    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));
    WORD wSum = 0;

    PBYTE pbLast = pbModule;
    for (;; pbLast = (PBYTE)mbi.BaseAddress + mbi.RegionSize) {
        ZeroMemory(&mbi, sizeof(mbi));
        if (VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) == 0) {
            if (GetLastError() == ERROR_INVALID_PARAMETER) {
                break;
            }
            DETOUR_TRACE(("VirtualQueryEx(%p) failed: %d\n",
                          pbLast, GetLastError()));
            break;
        }

        if (mbi.AllocationBase != pbModule) {
            break;
        }

        wSum = ChkSumRange(wSum,
                           hProcess,
                           (PBYTE)mbi.BaseAddress,
                           (PBYTE)mbi.BaseAddress + mbi.RegionSize);

        DETOUR_TRACE(("[%p..%p] : %04x\n",
                      (PBYTE)mbi.BaseAddress,
                      (PBYTE)mbi.BaseAddress + mbi.RegionSize,
                      wSum));
    }

    return detour_sum_final(wSum, pinh);
}
#endif // IGNORE_CHECKSUMS

//////////////////////////////////////////////////////////////////////////////
//
// Enumate through modules in the target process.
//
static HMODULE WINAPI EnumerateModulesInProcess(HANDLE hProcess,
                                                HMODULE hModuleLast,
                                                PIMAGE_NT_HEADERS32 pNtHeader)
{
    PBYTE pbLast;

    if (hModuleLast == NULL) {
        pbLast = (PBYTE)0x10000;
    }
    else {
        pbLast = (PBYTE)hModuleLast + 0x10000;
    }

    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));

    // Find the next memory region that contains a mapped PE image.
    //

    for (;; pbLast = (PBYTE)mbi.BaseAddress + mbi.RegionSize) {
        if (VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) == 0) {
            break;
        }
        if ((mbi.RegionSize & 0xfff) == 0xfff) {
            break;
        }
        if (((PBYTE)mbi.BaseAddress + mbi.RegionSize) < pbLast) {
            break;
        }

        // Skip uncommitted regions and guard pages.
        //
        if ((mbi.State != MEM_COMMIT) ||
            ((mbi.Protect & 0xff) == PAGE_NOACCESS) ||
            (mbi.Protect & PAGE_GUARD)) {
            continue;
        }

        __try {
            IMAGE_DOS_HEADER idh;
            if (!ReadProcessMemory(hProcess, pbLast, &idh, sizeof(idh), NULL)) {
                DETOUR_TRACE(("ReadProcessMemory(idh@%p..%p) failed: %d\n",
                              pbLast, pbLast + sizeof(idh), GetLastError()));
                continue;
            }

            if (idh.e_magic != IMAGE_DOS_SIGNATURE ||
                (DWORD)idh.e_lfanew > mbi.RegionSize ||
                (DWORD)idh.e_lfanew < sizeof(idh)) {
                continue;
            }

            if (!ReadProcessMemory(hProcess, pbLast + idh.e_lfanew,
                                   pNtHeader, sizeof(*pNtHeader), NULL)) {
                DETOUR_TRACE(("ReadProcessMemory(inh@%p..%p:%p) failed: %d\n",
                              pbLast + idh.e_lfanew,
                              pbLast + idh.e_lfanew + sizeof(*pNtHeader),
                              pbLast,
                              GetLastError()));
                continue;
            }

            if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) {
                continue;
            }

            return (HMODULE)pbLast;
        }
        __except(EXCEPTION_EXECUTE_HANDLER) {
            continue;
        }
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////
//
// Find a region of memory in which we can create a replacement import table.
//
static PBYTE FindAndAllocateNearBase(HANDLE hProcess, PBYTE pbBase, DWORD cbAlloc)
{
    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));

    PBYTE pbLast = pbBase;
    for (;; pbLast = (PBYTE)mbi.BaseAddress + mbi.RegionSize) {

        ZeroMemory(&mbi, sizeof(mbi));
        if (VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) == 0) {
            if (GetLastError() == ERROR_INVALID_PARAMETER) {
                break;
            }
            DETOUR_TRACE(("VirtualQueryEx(%p) failed: %d\n",
                          pbLast, GetLastError()));
            break;
        }
        if ((mbi.RegionSize & 0xfff) == 0xfff) {
            break;
        }

        // Skip anything other than a pure free region.
        //
        if (mbi.State != MEM_FREE) {
            continue;
        }

        PBYTE pbAddress = (PBYTE)(((DWORD_PTR)mbi.BaseAddress + 0xffff) & ~(DWORD_PTR)0xffff);

        DETOUR_TRACE(("Free region %p..%p\n",
                      mbi.BaseAddress,
                      (PBYTE)mbi.BaseAddress + mbi.RegionSize));

        for (; pbAddress < (PBYTE)mbi.BaseAddress + mbi.RegionSize; pbAddress += 0x10000) {
            PBYTE pbAlloc = (PBYTE)VirtualAllocEx(hProcess, pbAddress, cbAlloc,
                                                  MEM_RESERVE, PAGE_READWRITE);
            if (pbAlloc == NULL) {
                DETOUR_TRACE(("VirtualAllocEx(%p) failed: %d\n", pbAddress, GetLastError()));
                continue;
            }
            pbAlloc = (PBYTE)VirtualAllocEx(hProcess, pbAddress, cbAlloc,
                                            MEM_COMMIT, PAGE_READWRITE);
            if (pbAlloc == NULL) {
                DETOUR_TRACE(("VirtualAllocEx(%p) failed: %d\n", pbAddress, GetLastError()));
                continue;
            }
            DETOUR_TRACE(("[%p..%p] Allocated for import table.\n",
                          pbAlloc, pbAlloc + cbAlloc));
            return pbAlloc;
        }
    }
    return NULL;
}

static inline DWORD PadToDword(DWORD dw)
{
    return (dw + 3) & ~3u;
}

static inline DWORD PadToDwordPtr(DWORD dw)
{
    return (dw + 7) & ~7u;
}

static inline HRESULT ReplaceOptionalSizeA(char* pszDest,
                                           size_t cchDest,
                                           const char* pszSize)
{
    if (cchDest == 0 || pszDest == NULL || pszSize == NULL ||
        pszSize[0] == '\0' || pszSize[1] == '\0' || pszSize[2] != '\0') {

        // can not write into empty buffer or with string other than two chars.
        return ERROR_INVALID_PARAMETER;
    }
    else {
        for (; cchDest >= 2; cchDest--, pszDest++) {
            if (pszDest[0] == '?' && pszDest[1] == '?') {
                pszDest[0] = pszSize[0];
                pszDest[1] = pszSize[1];
                break;
            }
        }
    }
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
#if DETOURS_32BIT
#define DWORD_XX                        DWORD32
#define IMAGE_NT_HEADERS_XX             IMAGE_NT_HEADERS32
#define IMAGE_NT_OPTIONAL_HDR_MAGIC_XX  IMAGE_NT_OPTIONAL_HDR32_MAGIC
#define IMAGE_ORDINAL_FLAG_XX           IMAGE_ORDINAL_FLAG32
#define UPDATE_IMPORTS_XX               UpdateImports32
#define DETOURS_BITS_XX                 32
#include "uimports.cpp"
#undef DETOUR_EXE_RESTORE_FIELD_XX
#undef DWORD_XX
#undef IMAGE_NT_HEADERS_XX
#undef IMAGE_NT_OPTIONAL_HDR_MAGIC_XX
#undef IMAGE_ORDINAL_FLAG_XX
#undef UPDATE_IMPORTS_XX
#endif // DETOURS_32BIT

#if DETOURS_64BIT
#define DWORD_XX                        DWORD64
#define IMAGE_NT_HEADERS_XX             IMAGE_NT_HEADERS64
#define IMAGE_NT_OPTIONAL_HDR_MAGIC_XX  IMAGE_NT_OPTIONAL_HDR64_MAGIC
#define IMAGE_ORDINAL_FLAG_XX           IMAGE_ORDINAL_FLAG64
#define UPDATE_IMPORTS_XX               UpdateImports64
#define DETOURS_BITS_XX                 64
#include "uimports.cpp"
#undef DETOUR_EXE_RESTORE_FIELD_XX
#undef DWORD_XX
#undef IMAGE_NT_HEADERS_XX
#undef IMAGE_NT_OPTIONAL_HDR_MAGIC_XX
#undef IMAGE_ORDINAL_FLAG_XX
#undef UPDATE_IMPORTS_XX
#endif // DETOURS_64BIT

//////////////////////////////////////////////////////////////////////////////
//
#if DETOURS_64BIT

C_ASSERT(sizeof(IMAGE_NT_HEADERS64) == sizeof(IMAGE_NT_HEADERS32) + 16);

static BOOL UpdateFrom32To64(HANDLE hProcess, HANDLE hModule, WORD machine)
{
    IMAGE_DOS_HEADER idh;
    IMAGE_NT_HEADERS32 inh32;
    IMAGE_NT_HEADERS64 inh64;
    IMAGE_SECTION_HEADER sects[32];
    PBYTE pbModule = (PBYTE)hModule;
    DWORD n;

    ZeroMemory(&inh32, sizeof(inh32));
    ZeroMemory(&inh64, sizeof(inh64));
    ZeroMemory(sects, sizeof(sects));

    DETOUR_TRACE(("UpdateFrom32To64(%04x)\n", machine));
    //////////////////////////////////////////////////////// Read old headers.
    //
    if (!ReadProcessMemory(hProcess, pbModule, &idh, sizeof(idh), NULL)) {
        DETOUR_TRACE(("ReadProcessMemory(idh@%p..%p) failed: %d\n",
                      pbModule, pbModule + sizeof(idh), GetLastError()));
        return FALSE;
    }
    DETOUR_TRACE(("ReadProcessMemory(idh@%p..%p)\n",
                  pbModule, pbModule + sizeof(idh)));

    PBYTE pnh = pbModule + idh.e_lfanew;
    if (!ReadProcessMemory(hProcess, pnh, &inh32, sizeof(inh32), NULL)) {
        DETOUR_TRACE(("ReadProcessMemory(inh@%p..%p) failed: %d\n",
                      pnh, pnh + sizeof(inh32), GetLastError()));
        return FALSE;
    }
    DETOUR_TRACE(("ReadProcessMemory(inh@%p..%p)\n", pnh, pnh + sizeof(inh32)));

    if (inh32.FileHeader.NumberOfSections > (sizeof(sects)/sizeof(sects[0]))) {
        return FALSE;
    }

    PBYTE psects = pnh +
        FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) +
        inh32.FileHeader.SizeOfOptionalHeader;
    ULONG cb = inh32.FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
    if (!ReadProcessMemory(hProcess, psects, &sects, cb, NULL)) {
        DETOUR_TRACE(("ReadProcessMemory(ish@%p..%p) failed: %d\n",
                      psects, psects + cb, GetLastError()));
        return FALSE;
    }
    DETOUR_TRACE(("ReadProcessMemory(ish@%p..%p)\n", psects, psects + cb));

    ////////////////////////////////////////////////////////// Convert header.
    //
    inh64.Signature = inh32.Signature;
    inh64.FileHeader = inh32.FileHeader;
    inh64.FileHeader.Machine = machine;
    inh64.FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER64);

    inh64.OptionalHeader.Magic = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
    inh64.OptionalHeader.MajorLinkerVersion = inh32.OptionalHeader.MajorLinkerVersion;
    inh64.OptionalHeader.MinorLinkerVersion = inh32.OptionalHeader.MinorLinkerVersion;
    inh64.OptionalHeader.SizeOfCode = inh32.OptionalHeader.SizeOfCode;
    inh64.OptionalHeader.SizeOfInitializedData = inh32.OptionalHeader.SizeOfInitializedData;
    inh64.OptionalHeader.SizeOfUninitializedData = inh32.OptionalHeader.SizeOfUninitializedData;
    inh64.OptionalHeader.AddressOfEntryPoint = inh32.OptionalHeader.AddressOfEntryPoint;
    inh64.OptionalHeader.BaseOfCode = inh32.OptionalHeader.BaseOfCode;
    inh64.OptionalHeader.ImageBase = inh32.OptionalHeader.ImageBase;
    inh64.OptionalHeader.SectionAlignment = inh32.OptionalHeader.SectionAlignment;
    inh64.OptionalHeader.FileAlignment = inh32.OptionalHeader.FileAlignment;
    inh64.OptionalHeader.MajorOperatingSystemVersion
        = inh32.OptionalHeader.MajorOperatingSystemVersion;
    inh64.OptionalHeader.MinorOperatingSystemVersion
        = inh32.OptionalHeader.MinorOperatingSystemVersion;
    inh64.OptionalHeader.MajorImageVersion = inh32.OptionalHeader.MajorImageVersion;
    inh64.OptionalHeader.MinorImageVersion = inh32.OptionalHeader.MinorImageVersion;
    inh64.OptionalHeader.MajorSubsystemVersion = inh32.OptionalHeader.MajorSubsystemVersion;
    inh64.OptionalHeader.MinorSubsystemVersion = inh32.OptionalHeader.MinorSubsystemVersion;
    inh64.OptionalHeader.Win32VersionValue = inh32.OptionalHeader.Win32VersionValue;
    inh64.OptionalHeader.SizeOfImage = inh32.OptionalHeader.SizeOfImage;
    inh64.OptionalHeader.SizeOfHeaders = inh32.OptionalHeader.SizeOfHeaders;
    inh64.OptionalHeader.CheckSum = inh32.OptionalHeader.CheckSum;
    inh64.OptionalHeader.Subsystem = inh32.OptionalHeader.Subsystem;
    inh64.OptionalHeader.DllCharacteristics = inh32.OptionalHeader.DllCharacteristics;
    inh64.OptionalHeader.SizeOfStackReserve = inh32.OptionalHeader.SizeOfStackReserve;
    inh64.OptionalHeader.SizeOfStackCommit = inh32.OptionalHeader.SizeOfStackCommit;
    inh64.OptionalHeader.SizeOfHeapReserve = inh32.OptionalHeader.SizeOfHeapReserve;
    inh64.OptionalHeader.SizeOfHeapCommit = inh32.OptionalHeader.SizeOfHeapCommit;
    inh64.OptionalHeader.LoaderFlags = inh32.OptionalHeader.LoaderFlags;
    inh64.OptionalHeader.NumberOfRvaAndSizes = inh32.OptionalHeader.NumberOfRvaAndSizes;
    for (n = 0; n < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; n++) {
        inh64.OptionalHeader.DataDirectory[n] = inh32.OptionalHeader.DataDirectory[n];
    }

    inh64.IMPORT_DIRECTORY.VirtualAddress = 0;
    inh64.IMPORT_DIRECTORY.Size = 0;

    /////////////////////////////////////////////////////// Write new headers.
    //
    DWORD dwProtect = 0;
    if (!VirtualProtectEx(hProcess, pbModule, inh64.OptionalHeader.SizeOfHeaders,
                          PAGE_EXECUTE_READWRITE, &dwProtect)) {
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, pnh, &inh64, sizeof(inh64), NULL)) {
        DETOUR_TRACE(("WriteProcessMemory(inh@%p..%p) failed: %d\n",
                      pnh, pnh + sizeof(inh64), GetLastError()));
        return FALSE;
    }
    DETOUR_TRACE(("WriteProcessMemory(inh@%p..%p)\n", pnh, pnh + sizeof(inh64)));

    psects = pnh +
        FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) +
        inh64.FileHeader.SizeOfOptionalHeader;
    cb = inh64.FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
    if (!WriteProcessMemory(hProcess, psects, &sects, cb, NULL)) {
        DETOUR_TRACE(("WriteProcessMemory(ish@%p..%p) failed: %d\n",
                      psects, psects + cb, GetLastError()));
        return FALSE;
    }
    DETOUR_TRACE(("WriteProcessMemory(ish@%p..%p)\n", psects, psects + cb));

    DWORD dwOld = 0;
    if (!VirtualProtectEx(hProcess, pbModule, inh64.OptionalHeader.SizeOfHeaders,
                          dwProtect, &dwOld)) {
        return FALSE;
    }

    return TRUE;
}
#endif // DETOURS_64BIT

//////////////////////////////////////////////////////////////////////////////
//
BOOL WINAPI DetourUpdateProcessWithDll(HANDLE hProcess, LPCSTR *plpDlls, DWORD nDlls)
{
    // Find the next memory region that contains a mapped PE image.
    //
    WORD mach32Bit = 0;
    WORD mach64Bit = 0;
    WORD exe32Bit = 0;
    HMODULE hModule = NULL;
    HMODULE hLast = NULL;

    for (;;) {
        IMAGE_NT_HEADERS32 inh;

        if ((hLast = EnumerateModulesInProcess(hProcess, hLast, &inh)) == NULL) {
            break;
        }

        DETOUR_TRACE(("%p  machine=%04x magic=%04x\n",
                      hLast, inh.FileHeader.Machine, inh.OptionalHeader.Magic));

        if ((inh.FileHeader.Characteristics & IMAGE_FILE_DLL) == 0) {
            hModule = hLast;
            if (inh.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
                exe32Bit = inh.FileHeader.Machine;
            }
            DETOUR_TRACE(("%p  Found EXE\n", hLast));
        }
        else {
            if (inh.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
                mach32Bit = inh.FileHeader.Machine;
            }
            else if (inh.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
                mach64Bit = inh.FileHeader.Machine;
            }
        }
    }
    DETOUR_TRACE(("    mach32Bit=%04x mach64Bit=%04x\n", mach32Bit, mach64Bit));

    if (hModule == NULL) {
        SetLastError(ERROR_INVALID_OPERATION);
        return FALSE;
    }

    // Save the various headers for DetourRestoreAfterWith.
    //
    DETOUR_EXE_RESTORE der;
    ZeroMemory(&der, sizeof(der));
    der.cb = sizeof(der);

    der.pidh = (PBYTE)hModule;
    der.cbidh = sizeof(der.idh);
    if (!ReadProcessMemory(hProcess, der.pidh, &der.idh, sizeof(der.idh), NULL)) {
        DETOUR_TRACE(("ReadProcessMemory(idh@%p..%p) failed: %d\n",
                      der.pidh, der.pidh + der.cbidh, GetLastError()));
        return FALSE;
    }
    DETOUR_TRACE(("IDH: %p..%p\n", der.pidh, der.pidh + der.cbidh));

    // We read the NT header in two passes to get the full size.
    // First we read just the Signature and FileHeader.
    der.pinh = der.pidh + der.idh.e_lfanew;
    der.cbinh = FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader);
    if (!ReadProcessMemory(hProcess, der.pinh, &der.inh, der.cbinh, NULL)) {
        DETOUR_TRACE(("ReadProcessMemory(inh@%p..%p) failed: %d\n",
                      der.pinh, der.pinh + der.cbinh, GetLastError()));
        return FALSE;
    }

    // Second we read the OptionalHeader and Section headers.
    der.cbinh = (FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) +
                 der.inh.FileHeader.SizeOfOptionalHeader +
                 der.inh.FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER));
#if DETOURS_64BIT
    if (exe32Bit && !mach32Bit) {
        // Include the Save the extra 16-bytes that will be overwritten with 64-bit header.
        der.cbinh += sizeof(IMAGE_NT_HEADERS64) - sizeof(IMAGE_NT_HEADERS32);
    }
#endif // DETOURS_64BIT

    if (der.cbinh > sizeof(der.raw)) {
        return FALSE;
    }

    if (!ReadProcessMemory(hProcess, der.pinh, &der.inh, der.cbinh, NULL)) {
        DETOUR_TRACE(("ReadProcessMemory(inh@%p..%p) failed: %d\n",
                      der.pinh, der.pinh + der.cbinh, GetLastError()));
        return FALSE;
    }
    DETOUR_TRACE(("INH: %p..%p\n", der.pinh, der.pinh + der.cbinh));

    // Third, we read the CLR header

    if (der.inh.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        if (der.inh32.CLR_DIRECTORY.VirtualAddress != 0 &&
            der.inh32.CLR_DIRECTORY.Size != 0) {
        }
        DETOUR_TRACE(("CLR32.VirtAddr=%x, CLR.Size=%x\n",
                      der.inh32.CLR_DIRECTORY.VirtualAddress,
                      der.inh32.CLR_DIRECTORY.Size));

        der.pclr = ((PBYTE)hModule) + der.inh32.CLR_DIRECTORY.VirtualAddress;
    }
    else if (der.inh.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        if (der.inh64.CLR_DIRECTORY.VirtualAddress != 0 &&
            der.inh64.CLR_DIRECTORY.Size != 0) {
        }

        DETOUR_TRACE(("CLR64.VirtAddr=%x, CLR.Size=%x\n",
                      der.inh64.CLR_DIRECTORY.VirtualAddress,
                      der.inh64.CLR_DIRECTORY.Size));

        der.pclr = ((PBYTE)hModule) + der.inh64.CLR_DIRECTORY.VirtualAddress;
    }

    if (der.pclr != 0) {
        der.cbclr = sizeof(der.clr);
        if (!ReadProcessMemory(hProcess, der.pclr, &der.clr, der.cbclr, NULL)) {
            DETOUR_TRACE(("ReadProcessMemory(clr@%p..%p) failed: %d\n",
                          der.pclr, der.pclr + der.cbclr, GetLastError()));
            return FALSE;
        }
        DETOUR_TRACE(("CLR: %p..%p\n", der.pclr, der.pclr + der.cbclr));
    }

    // Fourth, adjust for a 32-bit WOW64 process.

    if (exe32Bit && mach64Bit) {
        if (!der.pclr                       // Native binary
            || (der.clr.Flags & 1) == 0     // Or mixed-mode MSIL
            || (der.clr.Flags & 2) != 0) {  // Or 32BIT Required MSIL

            mach64Bit = 0;
            if (mach32Bit == 0) {
                mach32Bit = exe32Bit;
            }
        }
    }

    // Now decide if we can insert the detour.

#if DETOURS_32BIT
    if (!mach32Bit && mach64Bit) {
        // 64-bit native or 64-bit managed process.
        //
        // Can't detour a 64-bit process with 32-bit code.
        // Note: This happens for 32-bit PE binaries containing only
        // manage code that have been marked as 64-bit ready.
        //
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    else if (mach32Bit) {
        // 32-bit native or 32-bit managed process on any platform.
        if (!UpdateImports32(hProcess, hModule, plpDlls, nDlls)) {
            return FALSE;
        }
    }
    else {
        // Who knows!?
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif // DETOURS_32BIT

#if DETOURS_64BIT
    if (mach32Bit) {
        // Can't detour a 32-bit process with 64-bit code.
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    else if (exe32Bit && !mach32Bit) {
        // Try to convert the 32-bit managed binary to a 64-bit managed binary.
        if (!UpdateFrom32To64(hProcess, hModule, mach64Bit)) {
            return FALSE;
        }

        // 64-bit process from 32-bit managed binary.
        if (!UpdateImports64(hProcess, hModule, plpDlls, nDlls)) {
            return FALSE;
        }
    }
    else if (mach64Bit) {
        // 64-bit native or 64-bit managed process on any platform.
        if (!UpdateImports64(hProcess, hModule, plpDlls, nDlls)) {
            return FALSE;
        }
    }
    else {
        // Who knows!?
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
#endif // DETOURS_64BIT

    /////////////////////////////////////////////////// Update the CLR header.
    //
    if (der.pclr != NULL) {
        DETOUR_CLR_HEADER clr;
        CopyMemory(&clr, &der.clr, sizeof(clr));
        clr.Flags &= 0xfffffffe;    // Clear the IL_ONLY flag.

        DWORD dwProtect;
        if (!VirtualProtectEx(hProcess, der.pclr, sizeof(clr), PAGE_READWRITE, &dwProtect)) {
            DETOUR_TRACE(("VirtualProtectEx(clr) write failed: %d\n", GetLastError()));
            return FALSE;
        }

        if (!WriteProcessMemory(hProcess, der.pclr, &clr, sizeof(clr), NULL)) {
            DETOUR_TRACE(("WriteProcessMemory(clr) failed: %d\n", GetLastError()));
            return FALSE;
        }

        if (!VirtualProtectEx(hProcess, der.pclr, sizeof(clr), dwProtect, &dwProtect)) {
            DETOUR_TRACE(("VirtualProtectEx(clr) restore failed: %d\n", GetLastError()));
            return FALSE;
        }
        DETOUR_TRACE(("CLR: %p..%p\n", der.pclr, der.pclr + der.cbclr));

#if DETOURS_64BIT
        if (der.clr.Flags & 0x2) { // Is the 32BIT Required Flag set?
            // X64 never gets here because the process appears as a WOW64 process.
            // However, on IA64, it doesn't appear to be a WOW process.
            DETOUR_TRACE(("CLR Requires 32-bit\n", der.pclr, der.pclr + der.cbclr));
            SetLastError(ERROR_INVALID_HANDLE);
            return FALSE;
        }
#endif // DETOURS_64BIT
    }

    //////////////////////////////// Save the undo data to the target process.
    //
    if (!DetourCopyPayloadToProcess(hProcess, DETOUR_EXE_RESTORE_GUID, &der, sizeof(der))) {
        DETOUR_TRACE(("DetourCopyPayloadToProcess failed: %d\n", GetLastError()));
        return FALSE;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//
BOOL WINAPI DetourCreateProcessWithDllA(LPCSTR lpApplicationName,
                                        __in_z LPSTR lpCommandLine,
                                        LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                        LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                        BOOL bInheritHandles,
                                        DWORD dwCreationFlags,
                                        LPVOID lpEnvironment,
                                        LPCSTR lpCurrentDirectory,
                                        LPSTARTUPINFOA lpStartupInfo,
                                        LPPROCESS_INFORMATION lpProcessInformation,
                                        LPCSTR lpDllName,
                                        PDETOUR_CREATE_PROCESS_ROUTINEA pfCreateProcessA)
{
    DWORD dwMyCreationFlags = (dwCreationFlags | CREATE_SUSPENDED);
    PROCESS_INFORMATION pi;

    if (pfCreateProcessA == NULL) {
        pfCreateProcessA = CreateProcessA;
    }

    if (!pfCreateProcessA(lpApplicationName,
                          lpCommandLine,
                          lpProcessAttributes,
                          lpThreadAttributes,
                          bInheritHandles,
                          dwMyCreationFlags,
                          lpEnvironment,
                          lpCurrentDirectory,
                          lpStartupInfo,
                          &pi)) {
        return FALSE;
    }

    LPCSTR rlpDlls[2];
    DWORD nDlls = 0;
    if (lpDllName != NULL) {
        rlpDlls[nDlls++] = lpDllName;
    }

    if (!DetourUpdateProcessWithDll(pi.hProcess, rlpDlls, nDlls)) {
        TerminateProcess(pi.hProcess, ~0u);
        return FALSE;
    }

    if (lpProcessInformation) {
        CopyMemory(lpProcessInformation, &pi, sizeof(pi));
    }

    if (!(dwCreationFlags & CREATE_SUSPENDED)) {
        ResumeThread(pi.hThread);
    }
    return TRUE;
}


BOOL WINAPI DetourCreateProcessWithDllW(LPCWSTR lpApplicationName,
                                        __in_z LPWSTR lpCommandLine,
                                        LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                        LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                        BOOL bInheritHandles,
                                        DWORD dwCreationFlags,
                                        LPVOID lpEnvironment,
                                        LPCWSTR lpCurrentDirectory,
                                        LPSTARTUPINFOW lpStartupInfo,
                                        LPPROCESS_INFORMATION lpProcessInformation,
                                        LPCSTR lpDllName,
                                        PDETOUR_CREATE_PROCESS_ROUTINEW pfCreateProcessW)
{
    DWORD dwMyCreationFlags = (dwCreationFlags | CREATE_SUSPENDED);
    PROCESS_INFORMATION pi;

    if (pfCreateProcessW == NULL) {
        pfCreateProcessW = CreateProcessW;
    }

    if (!pfCreateProcessW(lpApplicationName,
                          lpCommandLine,
                          lpProcessAttributes,
                          lpThreadAttributes,
                          bInheritHandles,
                          dwMyCreationFlags,
                          lpEnvironment,
                          lpCurrentDirectory,
                          lpStartupInfo,
                          &pi)) {
        return FALSE;
    }

    LPCSTR rlpDlls[2];
    DWORD nDlls = 0;
    if (lpDllName != NULL) {
        rlpDlls[nDlls++] = lpDllName;
    }

    if (!DetourUpdateProcessWithDll(pi.hProcess, rlpDlls, nDlls)) {
        TerminateProcess(pi.hProcess, ~0u);
        return FALSE;
    }

    if (lpProcessInformation) {
        CopyMemory(lpProcessInformation, &pi, sizeof(pi));
    }

    if (!(dwCreationFlags & CREATE_SUSPENDED)) {
        ResumeThread(pi.hThread);
    }
    return TRUE;
}

BOOL WINAPI DetourCopyPayloadToProcess(HANDLE hProcess,
                                       REFGUID rguid,
                                       PVOID pData,
                                       DWORD cbData)
{
    DWORD cbTotal = (sizeof(IMAGE_DOS_HEADER) +
                     sizeof(IMAGE_NT_HEADERS) +
                     sizeof(IMAGE_SECTION_HEADER) +
                     sizeof(DETOUR_SECTION_HEADER) +
                     sizeof(DETOUR_SECTION_RECORD) +
                     cbData);

    PBYTE pbBase = (PBYTE)VirtualAllocEx(hProcess, NULL, cbTotal,
                                         MEM_COMMIT, PAGE_READWRITE);
    if (pbBase == NULL) {
        DETOUR_TRACE(("VirtualAllocEx(%d) failed: %d\n", cbTotal, GetLastError()));
        return FALSE;
    }

    PBYTE pbTarget = pbBase;
    IMAGE_DOS_HEADER idh;
    IMAGE_NT_HEADERS inh;
    IMAGE_SECTION_HEADER ish;
    DETOUR_SECTION_HEADER dsh;
    DETOUR_SECTION_RECORD dsr;
    SIZE_T cbWrote = 0;

    ZeroMemory(&idh, sizeof(idh));
    idh.e_magic = IMAGE_DOS_SIGNATURE;
    idh.e_lfanew = sizeof(idh);
    if (!WriteProcessMemory(hProcess, pbTarget, &idh, sizeof(idh), &cbWrote) ||
        cbWrote != sizeof(idh)) {
        DETOUR_TRACE(("WriteProcessMemory(idh) failed: %d\n", GetLastError()));
        return FALSE;
    }
    pbTarget += sizeof(idh);

    ZeroMemory(&inh, sizeof(inh));
    inh.Signature = IMAGE_NT_SIGNATURE;
    inh.FileHeader.SizeOfOptionalHeader = sizeof(inh.OptionalHeader);
    inh.FileHeader.Characteristics = IMAGE_FILE_DLL;
    inh.FileHeader.NumberOfSections = 1;
    inh.OptionalHeader.Magic = IMAGE_NT_OPTIONAL_HDR_MAGIC;
    if (!WriteProcessMemory(hProcess, pbTarget, &inh, sizeof(inh), &cbWrote) ||
        cbWrote != sizeof(inh)) {
        return FALSE;
    }
    pbTarget += sizeof(inh);

    ZeroMemory(&ish, sizeof(ish));
    memcpy(ish.Name, ".detour", sizeof(ish.Name));
    ish.VirtualAddress = (DWORD)((pbTarget + sizeof(ish)) - pbBase);
    ish.SizeOfRawData = (sizeof(DETOUR_SECTION_HEADER) +
                         sizeof(DETOUR_SECTION_RECORD) +
                         cbData);
    if (!WriteProcessMemory(hProcess, pbTarget, &ish, sizeof(ish), &cbWrote) ||
        cbWrote != sizeof(ish)) {
        return FALSE;
    }
    pbTarget += sizeof(ish);

    ZeroMemory(&dsh, sizeof(dsh));
    dsh.cbHeaderSize = sizeof(dsh);
    dsh.nSignature = DETOUR_SECTION_HEADER_SIGNATURE;
    dsh.nDataOffset = sizeof(DETOUR_SECTION_HEADER);
    dsh.cbDataSize = (sizeof(DETOUR_SECTION_HEADER) +
                      sizeof(DETOUR_SECTION_RECORD) +
                      cbData);
    if (!WriteProcessMemory(hProcess, pbTarget, &dsh, sizeof(dsh), &cbWrote) ||
        cbWrote != sizeof(dsh)) {
        return FALSE;
    }
    pbTarget += sizeof(dsh);

    ZeroMemory(&dsr, sizeof(dsr));
    dsr.cbBytes = cbData + sizeof(DETOUR_SECTION_RECORD);
    dsr.nReserved = 0;
    dsr.guid = rguid;
    if (!WriteProcessMemory(hProcess, pbTarget, &dsr, sizeof(dsr), &cbWrote) ||
        cbWrote != sizeof(dsr)) {
        return FALSE;
    }
    pbTarget += sizeof(dsr);

    if (!WriteProcessMemory(hProcess, pbTarget, pData, cbData, &cbWrote) ||
        cbWrote != cbData) {
        return FALSE;
    }
    pbTarget += cbData;

    DETOUR_TRACE(("Copied %d byte payload into target process at %p\n",
                  cbTotal, pbTarget - cbTotal));
    return TRUE;
}

static BOOL fSearchedForHelper = FALSE;
static PDETOUR_EXE_HELPER pHelper = NULL;

VOID CALLBACK DetourFinishHelperProcess(HWND, HINSTANCE, LPSTR, INT)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pHelper->pid);
    if (hProcess == NULL) {
        DETOUR_TRACE(("OpenProcess(pid=%d) failed: %d\n",
                      pHelper->pid, GetLastError()));
        ExitProcess(9901);
    }

    PCSTR pszModule = pHelper->DllName;
    if (!DetourUpdateProcessWithDll(hProcess, &pszModule, 1)) {
        DETOUR_TRACE(("DetourUpdateProcessWithDll(pid=%d) failed: %d\n",
                      pHelper->pid, GetLastError()));
        ExitProcess(9902);
    }
}

BOOL WINAPI DetourIsHelperProcess(VOID)
{
    PVOID pvData;
    DWORD cbData;

    if (fSearchedForHelper) {
        return (pHelper != NULL);
    }

    fSearchedForHelper = TRUE;
    pvData = DetourFindPayloadEx(DETOUR_EXE_HELPER_GUID, &cbData);

    if (pvData == NULL || cbData < sizeof(DETOUR_EXE_HELPER)) {
        return FALSE;
    }

    pHelper = (PDETOUR_EXE_HELPER)pvData;
    if (pHelper->cb < sizeof(*pHelper)) {
        pHelper = NULL;
        return FALSE;
    }

    return TRUE;
}

BOOL WINAPI DetourProcessViaHelperA(DWORD dwTargetPid,
                                    LPCSTR lpDllName,
                                    PDETOUR_CREATE_PROCESS_ROUTINEA pfCreateProcessA)
{
    PROCESS_INFORMATION pi;
    STARTUPINFOA si;
    CHAR szExe[MAX_PATH];
    CHAR szCommand[MAX_PATH];
    DETOUR_EXE_HELPER helper;

    ZeroMemory(&helper, sizeof(helper));
    helper.cb = sizeof(helper);
    helper.pid = dwTargetPid;
    strcpy_s(helper.DllName, ARRAYSIZE(helper.DllName), lpDllName);

    DWORD nLen = GetEnvironmentVariableA("WINDIR", szExe, ARRAYSIZE(szExe));
    if (nLen == 0 || nLen >= ARRAYSIZE(szExe)) {
        return FALSE;
    }

#if DETOURS_OPTION_BITS
#if DETOURS_32BIT
    strcat_s(szExe, ARRAYSIZE(szExe), "\\sysnative\\rundll32.exe");
#else // !DETOURS_32BIT
    strcat_s(szExe, ARRAYSIZE(szExe), "\\syswow64\\rundll32.exe");
#endif // !DETOURS_32BIT

    PCHAR pszDll;
    if ((pszDll = strrchr(helper.DllName, '\\')) != NULL) {
        pszDll++;
    }
    else if ((pszDll = strrchr(helper.DllName, ':')) != NULL) {
        pszDll++;
    }
    else {
        pszDll = helper.DllName;
    }

    // Replace "32." with "64." or "64." with "32."
    for (; *pszDll; pszDll++) {
#if DETOURS_32BIT
        if (pszDll[0] == '3' && pszDll[1] == '2' && pszDll[2] == '.') {
            pszDll[0] = '6'; pszDll[1] = '4';
            break;
        }
#else
        if (pszDll[0] == '6' && pszDll[1] == '4' && pszDll[2] == '.') {
            pszDll[0] = '3'; pszDll[1] = '2';
            break;
        }
#endif
    }

#else // DETOURS_OPTIONS_BITS
    strcat_s(szExe, ARRAYSIZE(szExe), "\\system32\\rundll32.exe");
#endif // DETOURS_OPTIONS_BITS

    sprintf_s(szCommand, ARRAYSIZE(szCommand),
              "rundll32.exe \"%hs\",#1", helper.DllName);

    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    if (pfCreateProcessA(szExe, szCommand, NULL, NULL, FALSE, CREATE_SUSPENDED,
                         NULL, NULL, &si, &pi)) {

        if (!DetourCopyPayloadToProcess(pi.hProcess,
                                        DETOUR_EXE_HELPER_GUID,
                                        &helper, sizeof(helper))) {
            DETOUR_TRACE(("DetourCopyPayloadToProcess failed: %d\n", GetLastError()));
            TerminateProcess(pi.hProcess, ~0u);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return FALSE;
        }

        ResumeThread(pi.hThread);
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD dwResult = 500;
        GetExitCodeProcess(pi.hProcess, &dwResult);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (dwResult != 0) {
            DETOUR_TRACE(("Rundll32.exe failed: result=%d\n", dwResult));
            return FALSE;
        }
        return TRUE;
    }
    else {
        DETOUR_TRACE(("CreateProcess failed: %d\n", GetLastError()));
        return FALSE;
    }
}

BOOL WINAPI DetourProcessViaHelperW(DWORD dwTargetPid,
                                    LPCSTR lpDllName,
                                    PDETOUR_CREATE_PROCESS_ROUTINEW pfCreateProcessW)
{
    PROCESS_INFORMATION pi;
    STARTUPINFOW si;
    WCHAR szExe[MAX_PATH];
    WCHAR szCommand[MAX_PATH];
    DETOUR_EXE_HELPER helper;

    ZeroMemory(&helper, sizeof(helper));
    helper.cb = sizeof(helper);
    helper.pid = dwTargetPid;
    strcpy_s(helper.DllName, ARRAYSIZE(helper.DllName), lpDllName);

    DWORD nLen = GetEnvironmentVariableW(L"WINDIR", szExe, ARRAYSIZE(szExe));
    if (nLen == 0 || nLen >= ARRAYSIZE(szExe)) {
        return FALSE;
    }

#if DETOURS_OPTION_BITS
#if DETOURS_32BIT
    wcscat_s(szExe, ARRAYSIZE(szExe), L"\\sysnative\\rundll32.exe");
#else // !DETOURS_32BIT
    wcscat_s(szExe, ARRAYSIZE(szExe), L"\\syswow64\\rundll32.exe");
#endif // !DETOURS_32BIT

    PCHAR pszDll;
    if ((pszDll = strrchr(helper.DllName, '\\')) != NULL) {
        pszDll++;
    }
    else if ((pszDll = strrchr(helper.DllName, ':')) != NULL) {
        pszDll++;
    }
    else {
        pszDll = helper.DllName;
    }

    // Replace "32." with "64." or "64." with "32."
    for (; *pszDll; pszDll++) {
#if DETOURS_32BIT
        if (pszDll[0] == '3' && pszDll[1] == '2' && pszDll[2] == '.') {
            pszDll[0] = '6'; pszDll[1] = '4';
            break;
        }
#else
        if (pszDll[0] == '6' && pszDll[1] == '4' && pszDll[2] == '.') {
            pszDll[0] = '3'; pszDll[1] = '2';
            break;
        }
#endif
    }

#else // DETOURS_OPTIONS_BITS
    wcscat_s(szExe, ARRAYSIZE(szExe), L"\\system32\\rundll32.exe");
#endif // DETOURS_OPTIONS_BITS

    swprintf_s(szCommand, ARRAYSIZE(szCommand),
               L"rundll32.exe \"%hs\",#1", helper.DllName);

    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    if (pfCreateProcessW(szExe, szCommand, NULL, NULL, FALSE, CREATE_SUSPENDED,
                         NULL, NULL, &si, &pi)) {

        if (!DetourCopyPayloadToProcess(pi.hProcess,
                                        DETOUR_EXE_HELPER_GUID,
                                        &helper, sizeof(helper))) {
            DETOUR_TRACE(("DetourCopyPayloadToProcess failed: %d\n", GetLastError()));
            TerminateProcess(pi.hProcess, ~0u);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return FALSE;
        }

        ResumeThread(pi.hThread);

        ResumeThread(pi.hThread);
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD dwResult = 500;
        GetExitCodeProcess(pi.hProcess, &dwResult);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (dwResult != 0) {
            DETOUR_TRACE(("Rundll32.exe failed: result=%d\n", dwResult));
            return FALSE;
        }
        return TRUE;
    }
    else {
        DETOUR_TRACE(("CreateProcess failed: %d\n", GetLastError()));
        return FALSE;
    }
}

BOOL WINAPI DetourCreateProcessWithDllExA(LPCSTR lpApplicationName,
                                          __in_z LPSTR lpCommandLine,
                                          LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                          LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                          BOOL bInheritHandles,
                                          DWORD dwCreationFlags,
                                          LPVOID lpEnvironment,
                                          LPCSTR lpCurrentDirectory,
                                          LPSTARTUPINFOA lpStartupInfo,
                                          LPPROCESS_INFORMATION lpProcessInformation,
                                          LPCSTR lpDllName,
                                          PDETOUR_CREATE_PROCESS_ROUTINEA
                                          pfCreateProcessA)
{
    if (pfCreateProcessA == NULL) {
        pfCreateProcessA = CreateProcessA;
    }

    PROCESS_INFORMATION backup;
    if (lpProcessInformation == NULL) {
        lpProcessInformation = &backup;
        ZeroMemory(&backup, sizeof(backup));
    }

    if (!pfCreateProcessA(lpApplicationName,
                          lpCommandLine,
                          lpProcessAttributes,
                          lpThreadAttributes,
                          bInheritHandles,
                          dwCreationFlags | CREATE_SUSPENDED,
                          lpEnvironment,
                          lpCurrentDirectory,
                          lpStartupInfo,
                          lpProcessInformation)) {
        return FALSE;
    }

    LPCSTR szDll = lpDllName;

    if (!DetourUpdateProcessWithDll(lpProcessInformation->hProcess, &szDll, 1) &&
        !DetourProcessViaHelperA(lpProcessInformation->dwProcessId,
                                 lpDllName,
                                 pfCreateProcessA)) {

        TerminateProcess(lpProcessInformation->hProcess, ~0u);
        CloseHandle(lpProcessInformation->hProcess);
        CloseHandle(lpProcessInformation->hThread);
        return FALSE;
    }

    if (!(dwCreationFlags & CREATE_SUSPENDED)) {
        ResumeThread(lpProcessInformation->hThread);
    }

    if (lpProcessInformation == &backup) {
        CloseHandle(lpProcessInformation->hProcess);
        CloseHandle(lpProcessInformation->hThread);
    }

    return TRUE;
}

BOOL WINAPI DetourCreateProcessWithDllExW(LPCWSTR lpApplicationName,
                                          __in_z LPWSTR lpCommandLine,
                                          LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                          LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                          BOOL bInheritHandles,
                                          DWORD dwCreationFlags,
                                          LPVOID lpEnvironment,
                                          LPCWSTR lpCurrentDirectory,
                                          LPSTARTUPINFOW lpStartupInfo,
                                          LPPROCESS_INFORMATION lpProcessInformation,
                                          LPCSTR lpDllName,
                                          PDETOUR_CREATE_PROCESS_ROUTINEW
                                          pfCreateProcessW)
{
    if (pfCreateProcessW == NULL) {
        pfCreateProcessW = CreateProcessW;
    }

    PROCESS_INFORMATION backup;
    if (lpProcessInformation == NULL) {
        lpProcessInformation = &backup;
        ZeroMemory(&backup, sizeof(backup));
    }

    if (!pfCreateProcessW(lpApplicationName,
                          lpCommandLine,
                          lpProcessAttributes,
                          lpThreadAttributes,
                          bInheritHandles,
                          dwCreationFlags | CREATE_SUSPENDED,
                          lpEnvironment,
                          lpCurrentDirectory,
                          lpStartupInfo,
                          lpProcessInformation)) {
        return FALSE;
    }


    LPCSTR sz = lpDllName;

    if (!DetourUpdateProcessWithDll(lpProcessInformation->hProcess, &sz, 1) &&
        !DetourProcessViaHelperW(lpProcessInformation->dwProcessId,
                                 lpDllName,
                                 pfCreateProcessW)) {

        TerminateProcess(lpProcessInformation->hProcess, ~0u);
        CloseHandle(lpProcessInformation->hProcess);
        CloseHandle(lpProcessInformation->hThread);
        return FALSE;
    }

    if (!(dwCreationFlags & CREATE_SUSPENDED)) {
        ResumeThread(lpProcessInformation->hThread);
    }

    if (lpProcessInformation == &backup) {
        CloseHandle(lpProcessInformation->hProcess);
        CloseHandle(lpProcessInformation->hThread);
    }
    return TRUE;
}

//
///////////////////////////////////////////////////////////////// End of File.
