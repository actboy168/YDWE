#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)

#include <windows.h>
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4231)
#endif
#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace NLuaAPI { namespace NLog {

	log4cxx::LoggerPtr gScriptLogger;

	static void LuaLogFatal(const std::string &str)
	{
		LOG4CXX_FATAL(gScriptLogger, str);
	}

	static void LuaLogError(const std::string &str)
	{
		LOG4CXX_ERROR(gScriptLogger, str);
	}

	static void LuaLogWarning(const std::string &str)
	{
		LOG4CXX_WARN(gScriptLogger, str);
	}

	static void LuaLogInfo(const std::string &str)
	{
		LOG4CXX_INFO(gScriptLogger, str);
	}

	static void LuaLogDebug(const std::string &str)
	{
		LOG4CXX_DEBUG(gScriptLogger, str);
	}

	static void LuaLogTrace(const std::string &str)
	{
		LOG4CXX_TRACE(gScriptLogger, str);
	}
}}

int luaopen_log(lua_State *pState)
{
	using namespace luabind;

	NLuaAPI::NLog::gScriptLogger = log4cxx::Logger::getLogger(LOG4CXX_STR("YDScript"));

	module(pState, "log")
	[
		def("fatal", &NLuaAPI::NLog::LuaLogFatal),
		def("error", &NLuaAPI::NLog::LuaLogError),
		def("warn",  &NLuaAPI::NLog::LuaLogWarning),
		def("info",  &NLuaAPI::NLog::LuaLogInfo),
		def("debug", &NLuaAPI::NLog::LuaLogDebug),
		def("trace", &NLuaAPI::NLog::LuaLogTrace)
	];

	return 0;
}
