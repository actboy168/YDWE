#include <base/hook/replacedll.h>
#include <memory>

namespace base { namespace hook {

#define IMPORT_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
#define BOUND_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT]
#define IAT_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT]

	static HMODULE EnumerateModulesInProcess(HANDLE hProcess, HMODULE hModuleLast, PIMAGE_NT_HEADERS32 pNtHeader)
	{
		MEMORY_BASIC_INFORMATION mbi = { 0 };
		for (PBYTE pbLast = (PBYTE)hModuleLast + 0x10000;; pbLast = (PBYTE)mbi.BaseAddress + mbi.RegionSize) {
			if (VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) <= 0) {
				break;
			}
			if (((PBYTE)mbi.BaseAddress + mbi.RegionSize) < pbLast) {
				break;
			}
			if ((mbi.State != MEM_COMMIT) ||
				((mbi.Protect & 0xff) == PAGE_NOACCESS) ||
				(mbi.Protect & PAGE_GUARD)) {
				continue;
			}
			__try {
				IMAGE_DOS_HEADER idh;
				if (!ReadProcessMemory(hProcess, pbLast, &idh, sizeof(idh), NULL)) {
					continue;
				}
				if (idh.e_magic != IMAGE_DOS_SIGNATURE || (DWORD)idh.e_lfanew > mbi.RegionSize || (DWORD)idh.e_lfanew < sizeof(idh)) {
					continue;
				}
				if (!ReadProcessMemory(hProcess, pbLast + idh.e_lfanew, pNtHeader, sizeof(*pNtHeader), NULL)) {
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

	static PVOID FindExe(HANDLE hProcess)
	{
		HMODULE hModule = NULL;
		HMODULE hLast = NULL;
		for (;;) {
			IMAGE_NT_HEADERS32 inh;
			if ((hLast = EnumerateModulesInProcess(hProcess, hLast, &inh)) == NULL) {
				break;
			}
			if ((inh.FileHeader.Characteristics & IMAGE_FILE_DLL) == 0) {
				hModule = hLast;
			}
		}
		return hModule;
	}

	static PBYTE FindAndAllocateNearBase(HANDLE hProcess, PBYTE pbBase, DWORD cbAlloc)
	{
		MEMORY_BASIC_INFORMATION mbi = { 0 };
		for (PBYTE pbLast = pbBase;; pbLast = (PBYTE)mbi.BaseAddress + mbi.RegionSize) {
			ZeroMemory(&mbi, sizeof(mbi));
			if (VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) <= 0) {
				break;
			}
			if (mbi.State != MEM_FREE) {
				continue;
			}
			for (PBYTE pbAddress = (PBYTE)(((DWORD_PTR)mbi.BaseAddress + 0xffff) & ~(DWORD_PTR)0xffff) ; pbAddress < (PBYTE)mbi.BaseAddress + mbi.RegionSize; pbAddress += 0x10000) {
				PBYTE pbAlloc = (PBYTE)VirtualAllocEx(hProcess, pbAddress, cbAlloc, MEM_RESERVE, PAGE_READWRITE);
				if (pbAlloc == NULL) {
					continue;
				}
				pbAlloc = (PBYTE)VirtualAllocEx(hProcess, pbAddress, cbAlloc, MEM_COMMIT, PAGE_READWRITE);
				if (pbAlloc == NULL) {
					continue;
				}
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

	bool replacedll(const PROCESS_INFORMATION& pi, const char* oldDll, const char* newDll)
	{
		PBYTE pbModule = (PBYTE)FindExe(pi.hProcess);
		IMAGE_DOS_HEADER idh = { 0 };
		if (!ReadProcessMemory(pi.hProcess, pbModule, &idh, sizeof(idh), NULL)) {
			return false;
		}
		if (idh.e_magic != IMAGE_DOS_SIGNATURE) {
			return false;
		}

		IMAGE_NT_HEADERS inh = { 0 };
		if (!ReadProcessMemory(pi.hProcess, pbModule + idh.e_lfanew, &inh, sizeof(inh), NULL)) {
			return false;
		}
		if (inh.Signature != IMAGE_NT_SIGNATURE) {
			return false;
		}
		if (inh.IMPORT_DIRECTORY.VirtualAddress == 0) {
			return false;
		}
		inh.BOUND_DIRECTORY.VirtualAddress = 0;
		inh.BOUND_DIRECTORY.Size = 0;

		DWORD dwFileSize = 0;
		DWORD dwSec = idh.e_lfanew + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) + inh.FileHeader.SizeOfOptionalHeader;

		for (DWORD i = 0; i < inh.FileHeader.NumberOfSections; i++) {
			IMAGE_SECTION_HEADER ish = { 0 };
			if (!ReadProcessMemory(pi.hProcess, pbModule + dwSec + sizeof(ish) * i, &ish, sizeof(ish), NULL)) {
				return false;
			}
			if (inh.IAT_DIRECTORY.VirtualAddress == 0 && inh.IMPORT_DIRECTORY.VirtualAddress >= ish.VirtualAddress && inh.IMPORT_DIRECTORY.VirtualAddress < ish.VirtualAddress + ish.SizeOfRawData) {
				inh.IAT_DIRECTORY.VirtualAddress = ish.VirtualAddress;
				inh.IAT_DIRECTORY.Size = ish.SizeOfRawData;
			}
			if (dwFileSize < ish.PointerToRawData + ish.SizeOfRawData) {
				dwFileSize = ish.PointerToRawData + ish.SizeOfRawData;
			}
		}

		DWORD obStr = PadToDwordPtr(inh.IMPORT_DIRECTORY.Size);
		DWORD cbNew = obStr + PadToDword((DWORD)strlen(newDll) + 1);

		std::unique_ptr<BYTE> pbNew(new BYTE[cbNew]);
		ZeroMemory(pbNew.get(), cbNew);

		PBYTE pbBase = pbModule;
		PBYTE pbNext = pbBase
			+ inh.OptionalHeader.BaseOfCode
			+ inh.OptionalHeader.SizeOfCode
			+ inh.OptionalHeader.SizeOfInitializedData
			+ inh.OptionalHeader.SizeOfUninitializedData;
		if (pbBase < pbNext) {
			pbBase = pbNext;
		}

		PBYTE pbNewIid = FindAndAllocateNearBase(pi.hProcess, pbBase, cbNew);
		if (pbNewIid == NULL) {
			return false;
		}
		DWORD dwProtect = 0;
		if (!VirtualProtectEx(pi.hProcess, pbModule + inh.IMPORT_DIRECTORY.VirtualAddress, inh.IMPORT_DIRECTORY.Size, PAGE_EXECUTE_READWRITE, &dwProtect)) {
			return false;
		}
		DWORD obBase = (DWORD)(pbNewIid - pbModule);
		if (!ReadProcessMemory(pi.hProcess, pbModule + inh.IMPORT_DIRECTORY.VirtualAddress, pbNew.get(), inh.IMPORT_DIRECTORY.Size, NULL)) {
			return false;
		}
		PIMAGE_IMPORT_DESCRIPTOR piid = (PIMAGE_IMPORT_DESCRIPTOR)pbNew.get();
		errno_t err = strcpy_s((char*)pbNew.get() + obStr, cbNew - obStr, newDll);
		if (err) {
			return false;
		}
		CHAR szTemp[MAX_PATH];
		for (DWORD i = 0; i < inh.IMPORT_DIRECTORY.Size / sizeof(*piid); i++) {
			if (!ReadProcessMemory(pi.hProcess, pbModule + piid[i].Name, szTemp, MAX_PATH, NULL)) {
				return false;
			}
			if (_stricmp(szTemp, oldDll) == 0) {
				piid[i].Name = obBase + obStr;
			}
			if (piid[i].OriginalFirstThunk == 0 && piid[i].FirstThunk == 0) {
				break;
			}
		}
		if (!WriteProcessMemory(pi.hProcess, pbNewIid, pbNew.get(), cbNew, NULL)) {
			return false;
		}

		inh.IMPORT_DIRECTORY.VirtualAddress = obBase;
		inh.IMPORT_DIRECTORY.Size = cbNew;
		if (!VirtualProtectEx(pi.hProcess, pbModule, inh.OptionalHeader.SizeOfHeaders, PAGE_EXECUTE_READWRITE, &dwProtect)) {
			return false;
		}
		idh.e_res[0] = 0;
		if (!WriteProcessMemory(pi.hProcess, pbModule, &idh, sizeof(idh), NULL)) {
			return false;
		}
		if (!WriteProcessMemory(pi.hProcess, pbModule + idh.e_lfanew, &inh, sizeof(inh), NULL)) {
			return false;
		}
		if (!VirtualProtectEx(pi.hProcess, pbModule, inh.OptionalHeader.SizeOfHeaders, dwProtect, &dwProtect)) {
			return false;
		}
		return true;
	}
}}
