#include <Windows.h>
#include <locale>						  		
#include <base/filesystem.h>
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
			fs::path ydwe = base::path::self().remove_filename().remove_filename();
			if (gLuaEngine.Initialize(ydwe / L"logs"))
			{
				gLuaEngine.SetCPath(ydwe / L"bin" / L"modules" / L"?.dll"); 
				fs::path ydwedev = ydwe.parent_path().remove_filename().remove_filename();
				if (fs::exists(ydwedev / "build.root")) {
					ydwe = ydwedev / L"Component";
				}
				gLuaEngine.SetPath(ydwe / L"share" / L"script" / L"?.lua");
				gLuaEngine.LoadFile(ydwe / L"share" / L"script" / L"YDWE.lua");
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
