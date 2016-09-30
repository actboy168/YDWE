#include <Windows.h>
#include <locale>
#include <boost/filesystem.hpp>
#include <base/path/self.h>
#include "../LuaEngine/LuaEngine.h"

LuaEngine gLuaEngine;

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);

		try
		{
			std::locale::global(std::locale("", LC_CTYPE));
			boost::filesystem::path root_path = base::path::self().remove_filename().remove_filename();
			if (gLuaEngine.Initialize(root_path / L"logs"))
			{
				gLuaEngine.SetPath(root_path / L"share" / L"script" / L"?.lua");
				gLuaEngine.SetCPath(root_path / L"bin" / L"modules" / L"?.dll");
				gLuaEngine.LoadFile(root_path / L"share" / L"script" / L"YDWE.lua");
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
