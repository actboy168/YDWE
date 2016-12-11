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
			fs::path root_path = base::path::self().remove_filename().remove_filename();
			if (gLuaEngine.Initialize(root_path / L"logs"))
			{
				gLuaEngine.SetCPath(root_path / L"bin" / L"modules" / L"?.dll");
				fs::path dev_script = root_path.parent_path().remove_filename().remove_filename() / L"Component" / L"share" / L"script";
				if (fs::exists(dev_script)) {
					gLuaEngine.SetPath(dev_script / L"?.lua");
					gLuaEngine.LoadFile(dev_script / L"YDWE.lua");
				}
				else {
					gLuaEngine.SetPath(root_path / L"share" / L"script" / L"?.lua");
					gLuaEngine.LoadFile(root_path / L"share" / L"script" / L"YDWE.lua");
				}
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
