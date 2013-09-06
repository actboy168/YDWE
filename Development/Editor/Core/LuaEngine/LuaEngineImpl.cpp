#include <Windows.h>
#include "LuaEngineImpl.h"
#include <cstdint>
#include <aero/function/fp_call.hpp>
#include <boost/filesystem.hpp>
#include <ydwe/exception/exception.h>
#include <ydwe/hook/inline.h>
#include <ydwe/lua/luabind.h>
#include <ydwe/path/service.h>
#include <ydwe/path/self.h>
#include <ydwe/win/file_version.h>
#include <ydwe/file/stream.h>
#include <ydwe/util/format.h>

#pragma warning(push)
#pragma warning(disable: 4231)
#pragma warning(disable: 4251)
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/helpers/properties.h>
#include <log4cxx/helpers/fileinputstream.h>
#pragma warning(pop)

uintptr_t RealLuaPcall = (uintptr_t)::GetProcAddress(::GetModuleHandleW(L"luacore.dll"), "lua_pcall");
int FakeLuaPcall(lua_State *L, int nargs, int nresults, int errfunc)
{
	EXCEPTION_POINTERS* xp = nullptr;
	int results = 0;
	__try
	{
		results = aero::c_call<int>(RealLuaPcall, L, nargs, nresults, errfunc);
	}
	__except(xp = GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER)
	{
		char buf[256];
		sprintf(buf, "SEH exception: '0x%08X'", xp->ExceptionRecord->ExceptionCode);
		lua_pushstring(L, buf);
		return LUA_ERRRUN;
	}

	return results;
}

void LuaDoString(lua_State* pState, const char* str, size_t size, const char* name)
{
	if (luaL_loadbuffer(pState, str, size, name))
	{
		throw luabind::error(pState);
	}

	if (lua_pcall(pState, 0, LUA_MULTRET, 0))
	{
		throw luabind::error(pState);
	}
}

LuaEngineImpl::LuaEngineImpl()
	: state_(nullptr)
	, logger_(nullptr)
	, vaild_(false)
{ }

LuaEngineImpl::~LuaEngineImpl()
{
	state_= nullptr;
	logger_ = nullptr;
	vaild_ = false;
}

bool LuaEngineImpl::InitializeLogger(boost::filesystem::path const& log_config)
{
	log4cxx::helpers::Properties props;
	try
	{
		log4cxx::helpers::InputStreamPtr inputStream = new log4cxx::helpers::FileInputStream(log_config.c_str());
		props.load(inputStream);
	}
	catch (log4cxx::helpers::IOException &)
	{
		log4cxx::BasicConfigurator::configure();
		return false;
	}

	log4cxx::LogString log_file = props.getProperty(LOG4CXX_STR("log4j.appender.YDWELog.File"));
	if (!log_file.empty())
	{
		if (!boost::filesystem::path(log_file).is_absolute())
		{
			props.setProperty(LOG4CXX_STR("log4j.appender.YDWELog.File"), (log_config.parent_path() / log_file).wstring());
		}
	}

	log4cxx::PropertyConfigurator::configure(props);

	logger_ = log4cxx::Logger::getRootLogger();
	LOG4CXX_INFO(logger_, "------------------------------------------------------");

	return true;
}

bool LuaEngineImpl::InitializeInfo()
{
	OSVERSIONINFOW osvi = { sizeof OSVERSIONINFOW };
	::GetVersionExW(&osvi);

	LOG4CXX_INFO(logger_, ydwe::util::format(L"YDWE Script engine %s started.", ydwe::win::file_version(ydwe::path::self().c_str())[L"FileVersion"]));
	LOG4CXX_INFO(logger_, "Compiled at " __TIME__ ", " __DATE__);
	LOG4CXX_INFO(logger_, ydwe::util::format("Windows version: %d.%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber));

	return true;
}

bool LuaEngineImpl::InitializeLua()
{
#ifndef _DEBUG
	ydwe::hook::inline_install(&RealLuaPcall, (uintptr_t)FakeLuaPcall);
#endif
	state_ = luaL_newstate();
	if (!state_)
	{
		LOG4CXX_FATAL(logger_, "Could not initialize script engine. Program may not work correctly!");
		return false;
	}

	luabind::open(state_);
	luaL_openlibs(state_);
	LOG4CXX_DEBUG(logger_, "Initialize script engine successfully.");

	return true;
}

bool LuaEngineImpl::Initialize(boost::filesystem::path const& log_config)
{
	if (vaild_)
		return true;

	if (!InitializeLogger(log_config))
	{
		return false;
	}

	try
	{
		if (!InitializeInfo())
		{
			return false;
		}

		if (!InitializeLua())
		{
			return false;
		}

		LOG4CXX_INFO(logger_, "Script engine startup complete.");
		vaild_ = true;
		return true;
	}
	catch (std::exception const& e)
	{
		LOG4CXX_ERROR(logger_, "exception: " << e.what());
		return false;
	}
	catch (...)
	{
		LOG4CXX_ERROR(logger_, "unknown exception.");
		return false;
	}
}

bool LuaEngineImpl::Uninitialize()
{
	if (vaild_)
	{
		//lua_close(state_);
		state_ = nullptr;
		LOG4CXX_INFO(logger_, "Script engine has been shut down.");
		logger_ = nullptr;
		vaild_ = false;
	}

	return true;
}

bool LuaEngineImpl::LoadFile(boost::filesystem::path const& file_path)
{
	if (!vaild_)
	{
		return false;
	}

	std::string name;
	try {
		name = file_path.string();
	}
	catch (...) {
		name = "unknown";
	}

	try
	{
		std::vector<char> buffer = ydwe::file::read_stream(file_path).read<std::vector<char>>();
		LuaDoString(state_, buffer.data(), buffer.size(), name.c_str());
		return true;
	}
	catch (luabind::error const& e)
	{
		LOG4CXX_ERROR(logger_, "exception: " << lua_tostring(e.state(), -1));
		lua_pop(e.state(), 1);   
	}
	catch (std::exception const& e)
	{
		LOG4CXX_ERROR(logger_, "exception: " << e.what());
	}
	catch (...)
	{
		LOG4CXX_ERROR(logger_, "unknown exception.");
	}

	return false;
}

bool LuaEngineImpl::SetPath(boost::filesystem::path const& path)
{
	std::string path_str;
	try {
		path_str = path.string();
	}
	catch (...) {
		return false;
	}

	lua_getglobal(state_, "package");
	lua_pushstring(state_, path_str.c_str());
	lua_setfield(state_, -2, "path");
	lua_pop(state_, 1);
	return true;
}

bool LuaEngineImpl::SetCPath(boost::filesystem::path const& cpath)
{
	std::string cpath_str;
	try {
		cpath_str = cpath.string();
	}
	catch (...) {
		return false;
	}

	lua_getglobal(state_, "package");
	lua_pushstring(state_, cpath_str.c_str());
	lua_setfield(state_, -2, "cpath");
	lua_pop(state_, 1);
	return true;
}
