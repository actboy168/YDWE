#include <base/hook/replace_import.h>
#include <detours.h>

namespace base { namespace hook {

#if (_MSC_VER < 1299)
	typedef DWORD DWORD_PTR;
#endif
#if (_MSC_VER < 1310)
#else
#include <strsafe.h>
#endif

	// #define IGNORE_CHECKSUMS 1
#define DETOURS_INTERNAL
#define DETOUR_TRACE(x)

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
			if (VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) <= 0) {
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
			if (VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) <= 0) {
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
			__except (EXCEPTION_EXECUTE_HANDLER) {
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
			if (VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) <= 0) {
				if (GetLastError() == ERROR_INVALID_PARAMETER) {
					break;
				}
				DETOUR_TRACE(("VirtualQueryEx(%p) failed: %d\n",
					pbLast, GetLastError()));
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

	static BOOL UpdateImports32(HANDLE hProcess,
		HMODULE hModule,
		LPCSTR *plpDlls,
		DWORD nDlls)
	{
		BOOL fSucceeded = FALSE;
		BYTE * pbNew = NULL;
		DWORD i;

		PBYTE pbModule = (PBYTE)hModule;

		IMAGE_DOS_HEADER idh;
		ZeroMemory(&idh, sizeof(idh));
		if (!ReadProcessMemory(hProcess, pbModule, &idh, sizeof(idh), NULL)) {
			DETOUR_TRACE(("ReadProcessMemory(idh@%p..%p) failed: %d\n",
				pbModule, pbModule + sizeof(idh), GetLastError()));

		finish:
			if (pbNew != NULL) {
				delete[] pbNew;
				pbNew = NULL;
			}
			return fSucceeded;
		}

		IMAGE_NT_HEADERS32 inh;
		ZeroMemory(&inh, sizeof(inh));

		if (!ReadProcessMemory(hProcess, pbModule + idh.e_lfanew, &inh, sizeof(inh), NULL)) {
			DETOUR_TRACE(("ReadProcessMemory(inh@%p..%p) failed: %d\n",
				pbModule + idh.e_lfanew,
				pbModule + idh.e_lfanew + sizeof(inh),
				GetLastError()));
			goto finish;
		}

		if (inh.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
			DETOUR_TRACE(("Wrong size image (%04x != %04x).\n",
				inh.OptionalHeader.Magic, IMAGE_NT_OPTIONAL_HDR32_MAGIC));
			SetLastError(ERROR_INVALID_BLOCK);
			goto finish;
		}

		// Zero out the bound table so loader doesn't use it instead of our new table.
		inh.BOUND_DIRECTORY.VirtualAddress = 0;
		inh.BOUND_DIRECTORY.Size = 0;

		// Find the size of the mapped file.
		DWORD dwFileSize = 0;
		DWORD dwSec = idh.e_lfanew +
			FIELD_OFFSET(IMAGE_NT_HEADERS32, OptionalHeader) +
			inh.FileHeader.SizeOfOptionalHeader;

		for (i = 0; i < inh.FileHeader.NumberOfSections; i++) {
			IMAGE_SECTION_HEADER ish;
			ZeroMemory(&ish, sizeof(ish));

			if (!ReadProcessMemory(hProcess, pbModule + dwSec + sizeof(ish) * i, &ish,
				sizeof(ish), NULL)) {
				DETOUR_TRACE(("ReadProcessMemory(ish@%p..%p) failed: %d\n",
					pbModule + dwSec + sizeof(ish) * i,
					pbModule + dwSec + sizeof(ish) * (i + 1),
					GetLastError()));
				goto finish;
			}

			DETOUR_TRACE(("ish[%d] : va=%08x sr=%d\n", i, ish.VirtualAddress, ish.SizeOfRawData));

			// If the file didn't have an IAT_DIRECTORY, we assign it...
			if (inh.IAT_DIRECTORY.VirtualAddress == 0 &&
				inh.IMPORT_DIRECTORY.VirtualAddress >= ish.VirtualAddress &&
				inh.IMPORT_DIRECTORY.VirtualAddress < ish.VirtualAddress + ish.SizeOfRawData) {

				inh.IAT_DIRECTORY.VirtualAddress = ish.VirtualAddress;
				inh.IAT_DIRECTORY.Size = ish.SizeOfRawData;
			}

			// Find the end of the file...
			if (dwFileSize < ish.PointerToRawData + ish.SizeOfRawData) {
				dwFileSize = ish.PointerToRawData + ish.SizeOfRawData;
			}
		}
		DETOUR_TRACE(("dwFileSize = %08x\n", dwFileSize));

#if IGNORE_CHECKSUMS
		// Find the current checksum.
		WORD wBefore = ComputeChkSum(hProcess, pbModule, &inh);
		DETOUR_TRACE(("ChkSum: %04x + %08x => %08x\n", wBefore, dwFileSize, wBefore + dwFileSize));
#endif

		DETOUR_TRACE(("     Imports: %p..%p\n",
			(DWORD_PTR)pbModule + inh.IMPORT_DIRECTORY.VirtualAddress,
			(DWORD_PTR)pbModule + inh.IMPORT_DIRECTORY.VirtualAddress +
			inh.IMPORT_DIRECTORY.Size));

		DWORD obRem = sizeof(IMAGE_IMPORT_DESCRIPTOR) * nDlls;
		DWORD obTab = PadToDwordPtr(obRem +
			inh.IMPORT_DIRECTORY.Size +
			sizeof(IMAGE_IMPORT_DESCRIPTOR));
		DWORD obDll = obTab + sizeof(DWORD32) * 4 * nDlls;
		DWORD obStr = obDll;
		DWORD cbNew = obStr;
		DWORD n;
		for (n = 0; n < nDlls; n++) {
			cbNew += PadToDword((DWORD)strlen(plpDlls[n]) + 1);
		}

		pbNew = new BYTE[cbNew];
		if (pbNew == NULL) {
			DETOUR_TRACE(("new BYTE [cbNew] failed.\n"));
			goto finish;
		}
		ZeroMemory(pbNew, cbNew);

		PBYTE pbBase = pbModule;
		PBYTE pbNext = pbBase
			+ inh.OptionalHeader.BaseOfCode
			+ inh.OptionalHeader.SizeOfCode
			+ inh.OptionalHeader.SizeOfInitializedData
			+ inh.OptionalHeader.SizeOfUninitializedData;
		if (pbBase < pbNext) {
			pbBase = pbNext;
		}
		DETOUR_TRACE(("pbBase = %p\n", pbBase));

		PBYTE pbNewIid = FindAndAllocateNearBase(hProcess, pbBase, cbNew);
		if (pbNewIid == NULL) {
			DETOUR_TRACE(("FindAndAllocateNearBase failed.\n"));
			goto finish;
		}

		DWORD obBase = (DWORD)(pbNewIid - pbModule);
		DWORD dwProtect = 0;
		if (inh.IMPORT_DIRECTORY.VirtualAddress != 0) {
			// Read the old import directory if it exists.
			if (!VirtualProtectEx(hProcess,
				pbModule + inh.IMPORT_DIRECTORY.VirtualAddress,
				inh.IMPORT_DIRECTORY.Size, PAGE_EXECUTE_READWRITE, &dwProtect)) {
				DETOUR_TRACE(("VirtualProtectEx(import) write failed: %d\n", GetLastError()));
				goto finish;
			}
			DETOUR_TRACE(("IMPORT_DIRECTORY perms=%x\n", dwProtect));

			if (!ReadProcessMemory(hProcess,
				pbModule + inh.IMPORT_DIRECTORY.VirtualAddress,
				pbNew + obRem,
				inh.IMPORT_DIRECTORY.Size, NULL)) {
				DETOUR_TRACE(("ReadProcessMemory(imports) failed: %d\n", GetLastError()));
				goto finish;
			}
		}

		PIMAGE_IMPORT_DESCRIPTOR piid = (PIMAGE_IMPORT_DESCRIPTOR)pbNew;
		DWORD32 *pt;

		for (n = 0; n < nDlls; n++) {
			HRESULT hrRet = StringCchCopyA((char*)pbNew + obStr, cbNew - obStr, plpDlls[n]);
			if (FAILED(hrRet))
			{
				DETOUR_TRACE(("StringCchCopyA failed: %d\n", GetLastError()));
				goto finish;
			}

			DWORD nOffset = obTab + (sizeof(DWORD32) * (4 * n));
			piid[n].OriginalFirstThunk = obBase + nOffset;
			pt = ((DWORD32*)(pbNew + nOffset));
			pt[0] = IMAGE_ORDINAL_FLAG32 + 1;
			pt[1] = 0;

			nOffset = obTab + (sizeof(DWORD32) * ((4 * n) + 2));
			piid[n].FirstThunk = obBase + nOffset;
			pt = ((DWORD32*)(pbNew + nOffset));
			pt[0] = IMAGE_ORDINAL_FLAG32 + 1;
			pt[1] = 0;
			piid[n].TimeDateStamp = 0;
			piid[n].ForwarderChain = 0;
			piid[n].Name = obBase + obStr;

			obStr += PadToDword((DWORD)strlen(plpDlls[n]) + 1);
		}

		for (i = 0; i < nDlls + (inh.IMPORT_DIRECTORY.Size / sizeof(*piid)); i++) {
			DETOUR_TRACE(("%8d. Look=%08x Time=%08x Fore=%08x Name=%08x Addr=%08x\n",
				i,
				piid[i].OriginalFirstThunk,
				piid[i].TimeDateStamp,
				piid[i].ForwarderChain,
				piid[i].Name,
				piid[i].FirstThunk));
			if (piid[i].OriginalFirstThunk == 0 && piid[i].FirstThunk == 0) {
				break;
			}
		}

		if (!WriteProcessMemory(hProcess, pbNewIid, pbNew, obStr, NULL)) {
			DETOUR_TRACE(("WriteProcessMemory(iid) failed: %d\n", GetLastError()));
			goto finish;
		}

		DETOUR_TRACE(("obBaseBef = %08x..%08x\n",
			inh.IMPORT_DIRECTORY.VirtualAddress,
			inh.IMPORT_DIRECTORY.VirtualAddress + inh.IMPORT_DIRECTORY.Size));
		DETOUR_TRACE(("obBaseAft = %08x..%08x\n", obBase, obBase + obStr));

		// If the file doesn't have an IAT_DIRECTORY, we create it...
		if (inh.IAT_DIRECTORY.VirtualAddress == 0) {
			inh.IAT_DIRECTORY.VirtualAddress = obBase;
			inh.IAT_DIRECTORY.Size = cbNew;
		}

		inh.IMPORT_DIRECTORY.VirtualAddress = obBase;
		inh.IMPORT_DIRECTORY.Size = cbNew;

		/////////////////////// Update the NT header for the new import directory.
		/////////////////////////////// Update the DOS header to fix the checksum.
		//
		if (!VirtualProtectEx(hProcess, pbModule, inh.OptionalHeader.SizeOfHeaders,
			PAGE_EXECUTE_READWRITE, &dwProtect)) {
			DETOUR_TRACE(("VirtualProtectEx(inh) write failed: %d\n", GetLastError()));
			goto finish;
		}

#if IGNORE_CHECKSUMS
		idh.e_res[0] = 0;
#else
		inh.OptionalHeader.CheckSum = 0;
#endif // IGNORE_CHECKSUMS

		if (!WriteProcessMemory(hProcess, pbModule, &idh, sizeof(idh), NULL)) {
			DETOUR_TRACE(("WriteProcessMemory(idh) failed: %d\n", GetLastError()));
			goto finish;
		}
		DETOUR_TRACE(("WriteProcessMemory(idh:%p..%p)\n", pbModule, pbModule + sizeof(idh)));

		if (!WriteProcessMemory(hProcess, pbModule + idh.e_lfanew, &inh, sizeof(inh), NULL)) {
			DETOUR_TRACE(("WriteProcessMemory(inh) failed: %d\n", GetLastError()));
			goto finish;
		}
		DETOUR_TRACE(("WriteProcessMemory(inh:%p..%p)\n",
			pbModule + idh.e_lfanew,
			pbModule + idh.e_lfanew + sizeof(inh)));

#if IGNORE_CHECKSUMS
		WORD wDuring = ComputeChkSum(hProcess, pbModule, &inh);
		DETOUR_TRACE(("ChkSum: %04x + %08x => %08x\n", wDuring, dwFileSize, wDuring + dwFileSize));

		idh.e_res[0] = detour_sum_minus(idh.e_res[0], detour_sum_minus(wDuring, wBefore));

		if (!WriteProcessMemory(hProcess, pbModule, &idh, sizeof(idh), NULL)) {
			DETOUR_TRACE(("WriteProcessMemory(idh) failed: %d\n", GetLastError()));
			goto finish;
		}
#endif // IGNORE_CHECKSUMS

		if (!VirtualProtectEx(hProcess, pbModule, inh.OptionalHeader.SizeOfHeaders,
			dwProtect, &dwProtect)) {
			DETOUR_TRACE(("VirtualProtectEx(idh) restore failed: %d\n", GetLastError()));
			goto finish;
		}

#if IGNORE_CHECKSUMS
		WORD wAfter = ComputeChkSum(hProcess, pbModule, &inh);
		DETOUR_TRACE(("ChkSum: %04x + %08x => %08x\n", wAfter, dwFileSize, wAfter + dwFileSize));
		DETOUR_TRACE(("Before: %08x, After: %08x\n", wBefore + dwFileSize, wAfter + dwFileSize));

		if (wBefore != wAfter) {
			DETOUR_TRACE(("Restore of checksum failed %04x != %04x.\n", wBefore, wAfter));
			goto finish;
		}
#endif // IGNORE_CHECKSUMS

		fSucceeded = TRUE;
		goto finish;
	}

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
		}

		//////////////////////////////// Save the undo data to the target process.
		//
		if (!DetourCopyPayloadToProcess(hProcess, DETOUR_EXE_RESTORE_GUID, &der, sizeof(der))) {
			DETOUR_TRACE(("DetourCopyPayloadToProcess failed: %d\n", GetLastError()));
			return FALSE;
		}
		return TRUE;
	}

	BOOL WINAPI CreateProcessWithDllA(LPCSTR lpApplicationName,
		__in_z LPSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCSTR lpCurrentDirectory,
		LPSTARTUPINFOA lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation,
		LPCSTR lpDllName)
	{
		DWORD dwMyCreationFlags = (dwCreationFlags | CREATE_SUSPENDED);
		PROCESS_INFORMATION pi;

		if (!CreateProcessA(lpApplicationName,
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

	BOOL WINAPI CreateProcessWithDllW(LPCWSTR lpApplicationName,
		__in_z LPWSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCWSTR lpCurrentDirectory,
		LPSTARTUPINFOW lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation,
		LPCSTR lpDllName)
	{
		DWORD dwMyCreationFlags = (dwCreationFlags | CREATE_SUSPENDED);
		PROCESS_INFORMATION pi;

		if (!CreateProcessW(lpApplicationName,
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



	PVOID FindExe(HANDLE hProcess)
	{
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
				DETOUR_TRACE(("%p  Found EXE\n", hLast));
			}
		}

		return hModule;
	}

	BOOL WINAPI ReplaceImport(HANDLE hProcess, LPCSTR lpOldDll, LPCSTR lpNewDll)
	{
		BOOL fSucceeded = FALSE;
		BYTE * pbNew = NULL;
		DETOUR_EXE_RESTORE der;
		DWORD i;

		ZeroMemory(&der, sizeof(der));
		der.cb = sizeof(der);

		PBYTE pbModule = (PBYTE)FindExe(hProcess);

		IMAGE_DOS_HEADER idh;
		ZeroMemory(&idh, sizeof(idh));

		if (!ReadProcessMemory(hProcess, pbModule, &idh, sizeof(idh), NULL)) {
			DETOUR_TRACE(("ReadProcessMemory(idh) failed: %d\n", GetLastError()));

		finish:
			if (pbNew != NULL) {
				delete[] pbNew;
				pbNew = NULL;
			}
			return fSucceeded;
		}
		CopyMemory(&der.idh, &idh, sizeof(idh));
		der.pidh = (PBYTE)(pbModule);

		if (idh.e_magic != IMAGE_DOS_SIGNATURE) {
			goto finish;
		}

		IMAGE_NT_HEADERS inh;
		ZeroMemory(&inh, sizeof(inh));

		if (!ReadProcessMemory(hProcess, pbModule + idh.e_lfanew, &inh, sizeof(inh), NULL)) {
			DETOUR_TRACE(("ReadProcessMemory(inh) failed: %d\n", GetLastError()));
			goto finish;
		}
		CopyMemory(&der.inh, &inh, sizeof(inh));
		der.pinh = (PBYTE)(pbModule + idh.e_lfanew);

		if (inh.Signature != IMAGE_NT_SIGNATURE) {
			goto finish;
		}

		if (inh.IMPORT_DIRECTORY.VirtualAddress == 0) {
			DETOUR_TRACE(("No IMAGE_DIRECTORY_ENTRY_IMPORT\n"));
			goto finish;
		}

		// Zero out the bound table so loader doesn't use it instead of our new table.
		inh.BOUND_DIRECTORY.VirtualAddress = 0;
		inh.BOUND_DIRECTORY.Size = 0;

		// Find the size of the mapped file.
		DWORD dwFileSize = 0;
		DWORD dwSec = idh.e_lfanew +
			FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) +
			inh.FileHeader.SizeOfOptionalHeader;

		for (i = 0; i < inh.FileHeader.NumberOfSections; i++) {
			IMAGE_SECTION_HEADER ish;
			ZeroMemory(&ish, sizeof(ish));

			if (!ReadProcessMemory(hProcess, pbModule + dwSec + sizeof(ish) * i, &ish,
				sizeof(ish), NULL)) {
				DETOUR_TRACE(("ReadProcessMemory(inh) failed: %d\n", GetLastError()));
				goto finish;
			}

			DETOUR_TRACE(("ish[%d] : va=%p sr=%d\n", i, ish.VirtualAddress, ish.SizeOfRawData));

			// If the file didn't have an IAT_DIRECTORY, we create one...
			if (inh.IAT_DIRECTORY.VirtualAddress == 0 &&
				inh.IMPORT_DIRECTORY.VirtualAddress >= ish.VirtualAddress &&
				inh.IMPORT_DIRECTORY.VirtualAddress < ish.VirtualAddress + ish.SizeOfRawData) {

				inh.IAT_DIRECTORY.VirtualAddress = ish.VirtualAddress;
				inh.IAT_DIRECTORY.Size = ish.SizeOfRawData;
			}

			// Find the end of the file...
			if (dwFileSize < ish.PointerToRawData + ish.SizeOfRawData) {
				dwFileSize = ish.PointerToRawData + ish.SizeOfRawData;
			}
		}
		DETOUR_TRACE(("dwFileSize = %08x\n", dwFileSize));

#if IGNORE_CHECKSUMS
		// Find the current checksum.
		WORD wBefore = ComputeChkSum(hProcess, pbModule, &inh);
		DETOUR_TRACE(("ChkSum: %04x + %08x => %08x\n", wBefore, dwFileSize, wBefore + dwFileSize));
#endif

		DETOUR_TRACE(("     Imports: %8p..%8p\n",
			(DWORD_PTR)pbModule + inh.IMPORT_DIRECTORY.VirtualAddress,
			(DWORD_PTR)pbModule + inh.IMPORT_DIRECTORY.VirtualAddress +
			inh.IMPORT_DIRECTORY.Size));

		DWORD obStr = PadToDwordPtr(inh.IMPORT_DIRECTORY.Size);
		DWORD cbNew = obStr + PadToDword((DWORD)strlen(lpNewDll) + 1);

		pbNew = new BYTE[cbNew];
		if (pbNew == NULL) {
			DETOUR_TRACE(("new BYTE [cbNew] failed.\n"));
			goto finish;
		}
		ZeroMemory(pbNew, cbNew);

		PBYTE pbBase = pbModule;
		PBYTE pbNext = pbBase
			+ inh.OptionalHeader.BaseOfCode
			+ inh.OptionalHeader.SizeOfCode
			+ inh.OptionalHeader.SizeOfInitializedData
			+ inh.OptionalHeader.SizeOfUninitializedData;
		if (pbBase < pbNext) {
			pbBase = pbNext;
		}
		DETOUR_TRACE(("pbBase = %p\n", pbBase));

		PBYTE pbNewIid = FindAndAllocateNearBase(hProcess, pbBase, cbNew);
		if (pbNewIid == NULL) {
			DETOUR_TRACE(("FindAndAllocateNearBase failed.\n"));
			goto finish;
		}

		DWORD dwProtect = 0;
		//der.impDirProt = 0;
		if (!VirtualProtectEx(hProcess,
			pbModule + inh.IMPORT_DIRECTORY.VirtualAddress,
			inh.IMPORT_DIRECTORY.Size, PAGE_EXECUTE_READWRITE, &dwProtect)) {
			DETOUR_TRACE(("VirtualProtextEx(import) write failed: %d\n", GetLastError()));
			goto finish;
		}
		DETOUR_TRACE(("IMPORT_DIRECTORY perms=%x\n", dwProtect));
		//der.impDirProt = dwProtect;

		DWORD obBase = (DWORD)(pbNewIid - pbModule);

		if (!ReadProcessMemory(hProcess,
			pbModule + inh.IMPORT_DIRECTORY.VirtualAddress,
			pbNew,
			inh.IMPORT_DIRECTORY.Size, NULL)) {
			DETOUR_TRACE(("ReadProcessMemory(imports) failed: %d\n", GetLastError()));
			goto finish;
		}

		PIMAGE_IMPORT_DESCRIPTOR piid = (PIMAGE_IMPORT_DESCRIPTOR)pbNew;


		HRESULT hrRet = StringCchCopyA((char*)pbNew + obStr, cbNew - obStr, lpNewDll);
		if (FAILED(hrRet))
		{
			DETOUR_TRACE(("StringCchCopyA failed: %d\n", GetLastError()));
			goto finish;
		}

		CHAR szTemp[MAX_PATH];

		for (i = 0; i < inh.IMPORT_DIRECTORY.Size / sizeof(*piid); i++) {
			DETOUR_TRACE(("%8d. Look=%08x Time=%08x Fore=%08x Name=%08x Addr=%08x\n",
				i,
				piid[i].OriginalFirstThunk,
				piid[i].TimeDateStamp,
				piid[i].ForwarderChain,
				piid[i].Name,
				piid[i].FirstThunk));

			if (!ReadProcessMemory(hProcess,
				pbModule + piid[i].Name,
				szTemp,
				MAX_PATH, NULL)) {
				DETOUR_TRACE(("ReadProcessMemory(imports) failed: %d\n", GetLastError()));
				goto finish;
			}

			if (_stricmp(szTemp, lpOldDll) == 0) {
				piid[i].Name = obBase + obStr;
			}
			if (piid[i].OriginalFirstThunk == 0 && piid[i].FirstThunk == 0) {
				break;
			}
		}

		if (!WriteProcessMemory(hProcess, pbNewIid, pbNew, cbNew, NULL)) {
			DETOUR_TRACE(("WriteProcessMemory(iid) failed: %d\n", GetLastError()));
			goto finish;
		}

		DETOUR_TRACE(("obBase = %p..%p\n",
			inh.IMPORT_DIRECTORY.VirtualAddress,
			inh.IMPORT_DIRECTORY.VirtualAddress + inh.IMPORT_DIRECTORY.Size));
		DETOUR_TRACE(("obBase = %p..%p\n", obBase, obBase + cbNew));

		inh.IMPORT_DIRECTORY.VirtualAddress = obBase;
		inh.IMPORT_DIRECTORY.Size = cbNew;

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
		}

		/////////////////////// Update the NT header for the import new directory.
		/////////////////////////////// Update the DOS header to fix the checksum.
		//
		if (!VirtualProtectEx(hProcess, pbModule, inh.OptionalHeader.SizeOfHeaders,
			PAGE_EXECUTE_READWRITE, &dwProtect)) {
			DETOUR_TRACE(("VirtualProtextEx(inh) write failed: %d\n", GetLastError()));
			goto finish;
		}

		idh.e_res[0] = 0;
		if (!WriteProcessMemory(hProcess, pbModule, &idh, sizeof(idh), NULL)) {
			DETOUR_TRACE(("WriteProcessMemory(idh) failed: %d\n", GetLastError()));
			goto finish;
		}

		if (!WriteProcessMemory(hProcess, pbModule + idh.e_lfanew, &inh, sizeof(inh), NULL)) {
			DETOUR_TRACE(("WriteProcessMemory(inh) failed: %d\n", GetLastError()));
			goto finish;
		}

#if IGNORE_CHECKSUMS
		WORD wDuring = ComputeChkSum(hProcess, pbModule, &inh);
		DETOUR_TRACE(("ChkSum: %04x + %08x => %08x\n", wDuring, dwFileSize, wDuring + dwFileSize));

		idh.e_res[0] = detour_sum_minus(idh.e_res[0], detour_sum_minus(wDuring, wBefore));

		if (!WriteProcessMemory(hProcess, pbModule, &idh, sizeof(idh), NULL)) {
			DETOUR_TRACE(("WriteProcessMemory(idh) failed: %d\n", GetLastError()));
			goto finish;
		}
#endif

		if (!VirtualProtectEx(hProcess, pbModule, inh.OptionalHeader.SizeOfHeaders,
			dwProtect, &dwProtect)) {
			DETOUR_TRACE(("VirtualProtextEx(idh) restore failed: %d\n", GetLastError()));
			goto finish;
		}

#if IGNORE_CHECKSUMS
		WORD wAfter = ComputeChkSum(hProcess, pbModule, &inh);
		DETOUR_TRACE(("ChkSum: %04x + %08x => %08x\n", wAfter, dwFileSize, wAfter + dwFileSize));
		DETOUR_TRACE(("Before: %08x, After: %08x\n", wBefore + dwFileSize, wAfter + dwFileSize));

		if (wBefore != wAfter) {
			DETOUR_TRACE(("Restore of checksum failed %04x != %04x.\n", wBefore, wAfter));
			goto finish;
		}
#endif
		if (!DetourCopyPayloadToProcess(hProcess, DETOUR_EXE_RESTORE_GUID, &der, sizeof(der))) {
			DETOUR_TRACE(("DetourCopyPayloadToProcess failed: %d\n", GetLastError()));
			goto finish;
		}

		fSucceeded = TRUE;
		goto finish;
	}
}}
