#include <Windows.h>
#include <locale>						  		
#include <base/filesystem.h>
#include <base/path/self.h>
#include <base/util/dynarray.h>
#include <array>
#include "../LuaEngine/LuaEngine.h"

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
			fs::path ydwe = base::path::self().remove_filename().remove_filename();
			if (gLuaEngine.Initialize(ydwe / L"logs", name))
			{
				gLuaEngine.SetCPath(ydwe / L"bin" / L"modules" / L"?.dll");
				fs::path ydwedev = ydwe.parent_path().remove_filename().remove_filename();
				if (fs::exists(ydwedev / "build.root")) {
					ydwe = ydwedev / L"Component";
				}
				gLuaEngine.SetPath(ydwe / L"share" / L"script" / name / L"?.lua");
				gLuaEngine.LoadFile(ydwe / L"share" / L"script" / name / L"main.lua");
			}
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
