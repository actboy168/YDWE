#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <windows.h>
#include "YDWELogger.h"

namespace NYDWE
{
	log4cxx::LoggerPtr gInjectLogger;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
		NYDWE::gInjectLogger = log4cxx::Logger::getLogger(L"YDInject");
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		NYDWE::gInjectLogger = NULL;
	}

	return TRUE;
}
