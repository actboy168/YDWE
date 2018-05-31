#include <deque>
#include <windows.h>
#include <Shlwapi.h>

struct strview {
	const wchar_t* buf;
	size_t len;
	strview(const wchar_t* str);
	strview(const wchar_t* str, size_t len);
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
	template <class T, size_t n>
	void operator +=(T(&str)[n]) {
		append(str, n - 1);
	}
	template <size_t n>
	void operator +=(const strbuilder<n>& str) {
		append(str.buf, str.len);
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
	path(HMODULE m);
	template <class T, size_t n>
	path& operator /=(T(&str)[n]) {
		*this += L"\\";
		*this += str;
		return *this;
	}
	path& operator /=(const wchar_t* str) {
		*this += L"\\";
		*this += str;
		return *this;
	}
};

template <class T, size_t n>
path operator /(path& self, T(&str)[n]) {
	path res = self;
	res += L"\\";
	res += str;
	return res;
}

path operator /(path& self, const wchar_t* str) {
	path res = self;
	res += L"\\";
	res += str;
	return res;
}

strview::strview(const wchar_t* str)
	: buf(str)
	, len(wcslen(str))
{ }

strview::strview(const wchar_t* str, size_t len)
	: buf(str)
	, len(len)
{ }

path::path(HMODULE m) {
	::GetModuleFileNameW(m, buf, sizeof buf / sizeof buf[0]);
	::PathRemoveBlanksW(buf);
	::PathUnquoteSpacesW(buf);
	::PathRemoveBackslashW(buf);
	::PathRemoveFileSpecW(buf);
	len = wcslen(buf);
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
	path binPath = path(module);
	wchar_t buffer[MAX_PATH];
	::GetCurrentDirectoryW(sizeof(buffer) / sizeof(buffer[0]), buffer);
	::SetCurrentDirectoryW(binPath.string());
	for (const wchar_t *szDllName : szDllList)
	{
		hDllArray.push_front(::LoadLibraryW((binPath / szDllName).string()));
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
