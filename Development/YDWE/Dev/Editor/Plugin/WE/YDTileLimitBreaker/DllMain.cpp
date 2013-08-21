#include "DllMain.h"

namespace NYDWE
{

HMODULE gSelfModule;

log4cxx::LoggerPtr gInjectLogger;

} // namespace NYDWE

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	using namespace NYDWE;

	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
		gSelfModule = module;

		// Logger
		gInjectLogger = log4cxx::Logger::getLogger(LOG4CXX_STR("YDInject"));

		LOG4CXX_INFO(gInjectLogger, "YDTileLimitBreaker loaded.");
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		LOG4CXX_INFO(gInjectLogger, "YDTileLimitBreaker unloaded.");

		// Clean logger
		gInjectLogger = NULL;
	}

	return TRUE;
}

const char *PluginName()
{
	return "YDTileLimitBreaker";
}
