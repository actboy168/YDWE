#include <deque>
#include <filesystem>
#include <utility>
#include <windows.h>
#include <Shlwapi.h>
#include <base/filesystem.h>

static fs::path GetModuleDirectory(HMODULE hModule)
{
	fs::path result(L".");
	wchar_t buffer[MAX_PATH];

	if (::GetModuleFileNameW(hModule, buffer, sizeof(buffer) / sizeof(buffer[0])))
	{
		result = buffer;
		result.remove_filename(); // 获取上级路径
	}

	return std::move(result);
}

#ifdef _DEBUG

const wchar_t* szDllList[] = {
#if _MSC_VER == 1910
#elif _MSC_VER == 1800
	L"msvcr120d.dll",
	L"msvcp120d.dll",
#else  
	L"msvcr100d.dll",
	L"msvcp100d.dll",
#endif
	L"zlibd1.dll",
	L"StormLib.dll",
	L"luacore.dll", 
	L"ydbase.dll",
	L"LuaEngine.dll", // always at last
};

#else

const wchar_t* szDllList[] = {
#if _MSC_VER == 1910
#elif _MSC_VER == 1800
	L"msvcr120.dll",
	L"msvcp120.dll",
#elif  			 
	L"msvcr100.dll",
	L"msvcp100.dll",
#endif
	L"zlib1.dll",
	L"StormLib.dll",
	L"luacore.dll",	
	L"ydbase.dll",
	L"LuaEngine.dll", // always at last
};

#endif

std::deque<HMODULE> hDllArray;

void PreloadDll(const fs::path &ydweDirectory)
{
	if (!ydweDirectory.empty())
	{
		fs::path binPath = ydweDirectory.parent_path() / L"bin";

		wchar_t buffer[MAX_PATH];
		::GetCurrentDirectoryW(sizeof(buffer) / sizeof(buffer[0]), buffer);
		::SetCurrentDirectoryW(binPath.wstring().c_str());
		 
		for (const wchar_t *szDllName: szDllList)
		{
			hDllArray.push_front(::LoadLibraryW((binPath / szDllName).wstring().c_str()));
		}

		::SetCurrentDirectoryW(buffer);
	}
}

void PostfreeDll()
{
	for (HMODULE hDll : hDllArray)
	{
		if (hDll)
			::FreeLibrary(hDll);
	}
}

static void RestoreDetouredSystemDll(const fs::path &war3Directory)
{
	fs::path backupPath = war3Directory / L"ydwe_backups_system_dll";

	try
	{
		fs::directory_iterator endItr;
		for (fs::directory_iterator itr(backupPath); itr != endItr; ++itr)
		{
			try
			{
				if (!fs::is_directory(itr->path()))
				{
					fs::rename(itr->path(), war3Directory / itr->path().filename());
				}
			}
			catch(...)
			{}
		}
	}
	catch(...)
	{}

	try
	{
		fs::remove(backupPath);
	}
	catch (...)
	{}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);

		fs::path war3Directory = GetModuleDirectory(NULL);
		fs::path ydweDirectory = GetModuleDirectory(module);

		PreloadDll(ydweDirectory);
		RestoreDetouredSystemDll(war3Directory);
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		PostfreeDll();
	}

	return TRUE;
}

const char *PluginName()
{
	::PathFileExistsW(L"");
	return "YDDllFixer";
}
