#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)

#include <windows.h>
#include <LuaEngine\logging.h>

namespace NLuaAPI { namespace NLog {

	logging::logger lg;

	static void LuaLogFatal(const std::string &str)
	{
		LOGGING_FATAL(lg) << str;
	}

	static void LuaLogError(const std::string &str)
	{
		LOGGING_ERROR(lg) << str;
	}

	static void LuaLogWarning(const std::string &str)
	{
		LOGGING_WARNING(lg) << str;
	}

	static void LuaLogInfo(const std::string &str)
	{
		LOGGING_INFO(lg) << str;
	}

	static void LuaLogDebug(const std::string &str)
	{
		LOGGING_DEBUG(lg) << str;
	}

	static void LuaLogTrace(const std::string &str)
	{
		LOGGING_TRACE(lg) << str;
	}
}}

int luaopen_log(lua_State *pState)
{
	using namespace luabind;

	NLuaAPI::NLog::lg = logging::get_logger("lua");

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
