#include <windows.h>
#include "YDWELogger.h"
#include "YDWEHook.h"
#include "YDWEEditorNativeFunction.h"

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
		NYDWE::NativeInit();
		NYDWE::InstallHooks();
		LOG4CXX_INFO(NYDWE::gInjectLogger, "EverEditor startup complete.");
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		NYDWE::UninstallHooks();
		NYDWE::gInjectLogger = NULL;
	}

	return TRUE;
}


const char *PluginName()
{
	return "YDWeHelper";
}
