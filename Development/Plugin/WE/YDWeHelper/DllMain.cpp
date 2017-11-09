#include <windows.h>
#include "YDWELogger.h"
#include "YDWEHook.h"

namespace NYDWE
{
	logging::logger* lg;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);

		NYDWE::lg = logging::get_logger("inject");
		NYDWE::InstallHooks();
		LOGGING_INFO(NYDWE::lg) << "YDWE startup complete.";
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		NYDWE::UninstallHooks();
	}

	return TRUE;
}
