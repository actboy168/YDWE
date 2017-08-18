#include <base/hook/replace_import.h>
#include <memory>

namespace base { namespace hook {

#if (_MSC_VER < 1299)
	typedef DWORD DWORD_PTR;
#endif
#if (_MSC_VER < 1310)
#else
#include <strsafe.h>
#endif

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

	BOOL WINAPI ReplaceImport(HANDLE hProcess, LPCSTR lpOldDll, LPCSTR lpNewDll)
	{
		PBYTE pbModule = (PBYTE)FindExe(hProcess);
		IMAGE_DOS_HEADER idh = { 0 };
		if (!ReadProcessMemory(hProcess, pbModule, &idh, sizeof(idh), NULL)) {
			return FALSE;
		}
		if (idh.e_magic != IMAGE_DOS_SIGNATURE) {
			return FALSE;
		}

		IMAGE_NT_HEADERS inh = { 0 };
		if (!ReadProcessMemory(hProcess, pbModule + idh.e_lfanew, &inh, sizeof(inh), NULL)) {
			return FALSE;
		}
		if (inh.Signature != IMAGE_NT_SIGNATURE) {
			return FALSE;
		}
		if (inh.IMPORT_DIRECTORY.VirtualAddress == 0) {
			return FALSE;
		}
		inh.BOUND_DIRECTORY.VirtualAddress = 0;
		inh.BOUND_DIRECTORY.Size = 0;

		DWORD dwFileSize = 0;
		DWORD dwSec = idh.e_lfanew + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) + inh.FileHeader.SizeOfOptionalHeader;

		for (DWORD i = 0; i < inh.FileHeader.NumberOfSections; i++) {
			IMAGE_SECTION_HEADER ish = { 0 };
			if (!ReadProcessMemory(hProcess, pbModule + dwSec + sizeof(ish) * i, &ish, sizeof(ish), NULL)) {
				return FALSE;
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
		DWORD cbNew = obStr + PadToDword((DWORD)strlen(lpNewDll) + 1);

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

		PBYTE pbNewIid = FindAndAllocateNearBase(hProcess, pbBase, cbNew);
		if (pbNewIid == NULL) {
			return FALSE;
		}
		DWORD dwProtect = 0;
		if (!VirtualProtectEx(hProcess, pbModule + inh.IMPORT_DIRECTORY.VirtualAddress, inh.IMPORT_DIRECTORY.Size, PAGE_EXECUTE_READWRITE, &dwProtect)) {
			return FALSE;
		}
		DWORD obBase = (DWORD)(pbNewIid - pbModule);
		if (!ReadProcessMemory(hProcess, pbModule + inh.IMPORT_DIRECTORY.VirtualAddress, pbNew.get(), inh.IMPORT_DIRECTORY.Size, NULL)) {
			return FALSE;
		}
		PIMAGE_IMPORT_DESCRIPTOR piid = (PIMAGE_IMPORT_DESCRIPTOR)pbNew.get();
		HRESULT hrRet = StringCchCopyA((char*)pbNew.get() + obStr, cbNew - obStr, lpNewDll);
		if (FAILED(hrRet)) {
			return FALSE;
		}
		CHAR szTemp[MAX_PATH];
		for (DWORD i = 0; i < inh.IMPORT_DIRECTORY.Size / sizeof(*piid); i++) {
			if (!ReadProcessMemory(hProcess, pbModule + piid[i].Name, szTemp, MAX_PATH, NULL)) {
				return FALSE;
			}
			if (_stricmp(szTemp, lpOldDll) == 0) {
				piid[i].Name = obBase + obStr;
			}
			if (piid[i].OriginalFirstThunk == 0 && piid[i].FirstThunk == 0) {
				break;
			}
		}
		if (!WriteProcessMemory(hProcess, pbNewIid, pbNew.get(), cbNew, NULL)) {
			return FALSE;
		}

		inh.IMPORT_DIRECTORY.VirtualAddress = obBase;
		inh.IMPORT_DIRECTORY.Size = cbNew;
		if (!VirtualProtectEx(hProcess, pbModule, inh.OptionalHeader.SizeOfHeaders, PAGE_EXECUTE_READWRITE, &dwProtect)) {
			return FALSE;
		}
		idh.e_res[0] = 0;
		if (!WriteProcessMemory(hProcess, pbModule, &idh, sizeof(idh), NULL)) {
			return FALSE;
		}
		if (!WriteProcessMemory(hProcess, pbModule + idh.e_lfanew, &inh, sizeof(inh), NULL)) {
			return FALSE;
		}
		if (!VirtualProtectEx(hProcess, pbModule, inh.OptionalHeader.SizeOfHeaders, dwProtect, &dwProtect)) {
			return FALSE;
		}
		return TRUE;
	}
}}
