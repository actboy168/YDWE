#include <windows.h>
#include <Shlwapi.h>

struct strview {
	const wchar_t* buf;
	size_t len;
	strview(const wchar_t* str)
		: buf(str)
		, len(wcslen(str))
	{ }
	strview(const wchar_t* str, size_t len)
		: buf(str)
		, len(len)
	{ }
};

template <size_t N>
struct strbuilder {
	wchar_t buf[N];
	size_t  len = 0;
	void append(const wchar_t* str, size_t n) {
		if (len + n >= sizeof buf / sizeof buf[0]) {
			return;
		}
		wcsncpy(buf + len, str, n);
		len += n;
	}
	void operator +=(const strview& str) {
		append(str.buf, str.len);
	}
	wchar_t* string() {
		buf[len] = L'\0';
		return buf;
	}
};

struct path : public strbuilder<MAX_PATH> {
	path(HMODULE m) {
		::GetModuleFileNameW(m, buf, sizeof buf / sizeof buf[0]);
		::PathRemoveBlanksW(buf);
		::PathUnquoteSpacesW(buf);
		::PathRemoveBackslashW(buf);
		::PathRemoveFileSpecW(buf);
		len = wcslen(buf);
	}
};

path operator /(path& self, const wchar_t* str) {
	path res = self;
	res += L"\\";
	res += str;
	return res;
}

struct Dll{
	const wchar_t* name;
	HMODULE handle;
};

Dll hDllArray[] = {
#if !_DEBUG
	{ L"vcruntime140.dll",NULL },
	{ L"msvcp140.dll",NULL },
#endif
	{ L"zlib1.dll",NULL },
	{ L"StormLib.dll",NULL },
	{ L"luacore.dll",NULL },
	{ L"ydbase.dll",NULL },
	{ L"LuaEngine.dll",NULL },
};

void PreloadDll(HMODULE module)
{
	path binPath = path(module);
	wchar_t buffer[MAX_PATH];
	::GetCurrentDirectoryW(sizeof(buffer) / sizeof(buffer[0]), buffer);
	::SetCurrentDirectoryW(binPath.string());
	for (int i = 0; i < _countof(hDllArray); ++i)
	{
		auto dll = hDllArray[i];
		dll.handle = ::LoadLibraryW((binPath / dll.name).string());
	}
	::SetCurrentDirectoryW(buffer);
}

void PostfreeDll()
{
	for (int i = _countof(hDllArray); i > 0; --i)
	{
		auto dll = hDllArray[i - 1];
		if (dll.handle) {
			::FreeLibrary(dll.handle);
		}
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
