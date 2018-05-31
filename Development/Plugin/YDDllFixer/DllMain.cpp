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
		result = result.parent_path(); // 获取上级路径
	}

	return std::move(result);
}

const wchar_t* szDllList[] = {
#if !_DEBUG
	L"vcruntime140.dll",
	L"msvcp140.dll",
#endif
	L"zlib1.dll",
	L"StormLib.dll",
	L"luacore.dll",	
	L"ydbase.dll",
	L"LuaEngine.dll", // always at last
};

std::deque<HMODULE> hDllArray;

void PreloadDll(HMODULE module)
{
	fs::path ydweDirectory = GetModuleDirectory(module);
	fs::path binPath = ydweDirectory.parent_path() / L"bin";
	wchar_t buffer[MAX_PATH];
	::GetCurrentDirectoryW(sizeof(buffer) / sizeof(buffer[0]), buffer);
	::SetCurrentDirectoryW(binPath.wstring().c_str());
	for (const wchar_t *szDllName : szDllList)
	{
		hDllArray.push_front(::LoadLibraryW((binPath / szDllName).wstring().c_str()));
	}
	::SetCurrentDirectoryW(buffer);
}

void PostfreeDll()
{
	for (HMODULE hDll : hDllArray)
	{
		if (hDll)
			::FreeLibrary(hDll);
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);
		PreloadDll(module);
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
