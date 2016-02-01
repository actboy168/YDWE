#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <windows.h>
#include "YDWELogger.h"

namespace NYDWE
{
	logging::logger* lg;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
		NYDWE::lg = logging::get_logger("event");
	}

	return TRUE;
}
