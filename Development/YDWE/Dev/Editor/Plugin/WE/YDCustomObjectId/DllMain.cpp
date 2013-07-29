#include "CustomObjectIdHook.h"
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

		// Install hook
		NYDWE::InstallCustomObjectIdHook();

		LOG4CXX_INFO(gInjectLogger, "YDCustomObjectId loaded.");
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		LOG4CXX_INFO(gInjectLogger, "YDCustomObjectId unloaded.");

		// Uninstall hook
		NYDWE::UninstallCustomObjectIdHook();

		// Clean logger
		gInjectLogger = NULL;
	}

	return TRUE;
}

const char *PluginName()
{
	return "YDCustomObjectId";
}
