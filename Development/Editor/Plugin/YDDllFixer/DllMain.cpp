#include <utility>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <windows.h>
#include <Shlwapi.h>

namespace fs = boost::filesystem;

static fs::path GetModuleDirectory(HMODULE hModule)
{
	fs::path result(L".");
	wchar_t buffer[MAX_PATH];

	if (::GetModuleFileNameW(hModule, buffer, sizeof(buffer) / sizeof(buffer[0])))
	{
		result = buffer;
		result.remove_filename(); // ��ȡ�ϼ�·��
	}

	return std::move(result);
}

#ifdef _DEBUG

const wchar_t* szDllList[] = {
#if _MSC_VER == 1900
	L"msvcr120d.dll",
	L"msvcp140d.dll",
#elif  
	L"msvcr120d.dll",
	L"msvcp120d.dll",
#endif
	L"zlibd1.dll",
	L"StormLib.dll",
	L"sfmpq.dll",
	L"luacore.dll",
	L"luabind.dll", // luacore.dll	 
	L"ydbase.dll",
	L"LuaEngine.dll", // always at last
};

#else

const wchar_t* szDllList[] = {
#if _MSC_VER == 1900 
	L"msvcr120.dll",
	L"msvcp140.dll",
#elif  			 
	L"msvcr120.dll",
	L"msvcp120.dll",
#endif
	L"zlib1.dll",
	L"StormLib.dll",
	L"sfmpq.dll",
	L"luacore.dll",
	L"luabind.dll", // luacore.dll 		
	L"ydbase.dll",
	L"LuaEngine.dll", // always at last
};

#endif

HMODULE hDllArray[sizeof(szDllList) / sizeof(szDllList[0])];

void PreloadDll(const fs::path &ydweDirectory)
{
	if (!ydweDirectory.empty())
	{
		fs::path binPath = ydweDirectory.parent_path() / L"bin";

		wchar_t buffer[MAX_PATH];
		::GetCurrentDirectoryW(sizeof(buffer) / sizeof(buffer[0]), buffer);
		::SetCurrentDirectoryW(binPath.c_str());

		HMODULE *pMod = hDllArray;
		BOOST_FOREACH(const wchar_t *szDllName, szDllList)
		{
			*(pMod++) = ::LoadLibraryW((binPath / szDllName).c_str());
		}

		::SetCurrentDirectoryW(buffer);
	}
}

void PostfreeDll()
{
	BOOST_REVERSE_FOREACH(HMODULE hDll, hDllArray)
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
				if (!fs::is_directory(*itr))
				{
					fs::rename(*itr, war3Directory / itr->path().filename());
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
