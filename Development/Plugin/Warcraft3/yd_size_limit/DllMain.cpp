#include <windows.h>
#include <base/hook/fp_call.h> 
#include <base/hook/iat.h>
#include <base/warcraft3/war3_searcher.h>  
#include <base/warcraft3/version.h>
#include <base/file/memory_mapped_file.h>  	  		
#include <base/filesystem.h>
#include <Psapi.h>
#include <array>

#pragma comment(lib, "Psapi.lib")

bool oldversion = false;

static std::wstring GetFileExtFromHandle(HANDLE hFile)
{
	try {
		base::file::memory_mapped_file mapper(hFile, 0, 1);
		std::array<wchar_t, MAX_PATH + 1> filename;
		if (GetMappedFileNameW(GetCurrentProcess(), (void*)mapper.memory(), filename.data(), MAX_PATH))
		{
			std::wstring ext = fs::path(filename.data()).extension().wstring();
			std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
			return ext;
		}
	} catch (...) { }
	return std::wstring();
}

uintptr_t real_GetFileSize = 0;
DWORD __stdcall fake_GetFileSize(HANDLE file, LPDWORD lpFileSizeHigh)
{
	DWORD sizehi = 0;
	DWORD sizelo = base::std_call<DWORD>(real_GetFileSize, file, &sizehi);
	if (sizelo > 0 || sizehi > 0)
	{
		std::wstring ext = GetFileExtFromHandle(file);
		if (!ext.empty() && (ext == L".w3x" || ext == L".w3m"))
		{
			if (base::warcraft3::get_war3_searcher().get_version() >= base::warcraft3::version_124b)
			{
				if (sizelo > 0x7FFFFF || sizehi > 0)
				{
					sizelo = 0x7FFFFF;
					sizehi = 0;
				}
			}
			else
			{
				if (sizelo > 0x2FFFFF || sizehi > 0)
				{
					sizelo = 0x2FFFFF;
					sizehi = 0;
				}
			}
		}
	}
	if (lpFileSizeHigh) *lpFileSizeHigh = sizehi;
	return sizelo;
}

void Initialize()
{
	real_GetFileSize = base::hook::iat(L"storm.dll", "kernel32.dll", "GetFileSize", (uintptr_t)fake_GetFileSize);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}

const char *PluginName()
{
	return "yd_size_limit";
}
