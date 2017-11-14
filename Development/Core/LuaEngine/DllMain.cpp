#include <Windows.h>
#include <locale>
#include <array>
#include <base/util/dynarray.h>
#include "LuaEngine.h"

LuaEngine gLuaEngine;

std::wstring getenv(const wchar_t* varname)
{
	std::array<wchar_t, 4096> sbuf;
	DWORD r = GetEnvironmentVariableW(varname, sbuf.data(), sbuf.size());
	if (r <= sbuf.size()) {
		return std::wstring(sbuf.data(), r);
	}
	std::dynarray<wchar_t> dbuf(r);
	r = GetEnvironmentVariableW(varname, dbuf.data(), dbuf.size());
	if (r) {
		return std::wstring(dbuf.data(), r);
	}
	return std::wstring();
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);

		try
		{
			std::locale::global(std::locale("", LC_CTYPE));
			std::wstring name = getenv(L"ydwe-process-name");
			gLuaEngine.Initialize(name);
		}
		catch (...)
		{
			return FALSE;
		}
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		gLuaEngine.Uninitialize();
	}

	return TRUE;
}
