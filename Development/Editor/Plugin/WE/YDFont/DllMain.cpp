#include <windows.h>
#include "FontManager.hpp"
#include <base/path/self.h>

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);
		MessageBox(0, 0, 0, 0);
		g_fontptr.reset(new FontManager(base::path::self().parent_path()));
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
