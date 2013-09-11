#include <windows.h>
#include "FontManager.hpp"
#include <base/path/service.h>

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);
		g_fontptr.reset(new FontManager(ydwe::path::get(ydwe::path::DIR_MODULE).parent_path()));
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		g_fontptr.release();
	}
	return TRUE;
}

const char *PluginName()
{
	return "YDFont";
}
