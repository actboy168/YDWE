#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4231)
#endif
#include <log4cxx/logmanager.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <windows.h>
#include <shlwapi.h>

#include "YDJAPI.h"
#include "DllMain.h"

namespace fs = boost::filesystem;

#pragma comment(lib, "shlwapi.lib")

namespace NYDWE
{

HMODULE gSelfModule;

log4cxx::LoggerPtr gJAPIFrameworkLogger;

} // namespace NYDWE

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	using namespace NYDWE;

	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
		gSelfModule = module;

		// Initialize locale
		std::locale::global(std::locale("", LC_CTYPE));

		// Self path
		WCHAR buffer[MAX_PATH];
		GetModuleFileNameW(module, buffer, sizeof(buffer) / sizeof(buffer[0]));
		PathRemoveBlanksW(buffer);
		PathUnquoteSpacesW(buffer);
		PathRemoveBackslashW(buffer);

		bool isAutoCalledByMSSEngine = boost::iends_with(buffer, L".m3d") || boost::iends_with(buffer, L".asi")
			|| boost::iends_with(buffer, L".flt");

		// Initialize logger
		bool isConfigured = log4cxx::LogManager::getLoggerRepository()->isConfigured();
		if (!isConfigured)
		{
			// If exists configuration file, use it
			PathRemoveFileSpecW(buffer);
			PathAppendW(buffer, L"YDJAPILogger.cfg");

			if (PathFileExistsW(buffer))
			{
				log4cxx::PropertyConfigurator::configure(buffer);

				isConfigured = true;
			}
			else
			{
				// Default configuration
				log4cxx::BasicConfigurator::configure();
			}
		}

		gJAPIFrameworkLogger = log4cxx::Logger::getLogger(LOG4CXX_STR("YDJAPIFramework"));

		// Disable log if no configuration
		if (!isConfigured)
		{
			gJAPIFrameworkLogger->setLevel(log4cxx::Level::getOff());
		}

		if (isAutoCalledByMSSEngine)
		{
			LOG4CXX_DEBUG(gJAPIFrameworkLogger, "Called by MSS sound engine, now try auto initialize.");

			HMODULE gameDll = GetModuleHandleW(L"game.dll");
			if (gameDll)
				InitJAPIFramework(gameDll);
			else
				LOG4CXX_INFO(gJAPIFrameworkLogger, "Cannot find game.dll in process. Maybe we are not called by warcraft.");
		}

		LOG4CXX_INFO(gJAPIFrameworkLogger, "YDWE JAPI framework loaded.");
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		LOG4CXX_INFO(gJAPIFrameworkLogger, "YDWE JAPI framework unloaded.");

		// Clean logger
		gJAPIFrameworkLogger = NULL;
	}

	return TRUE;
}
