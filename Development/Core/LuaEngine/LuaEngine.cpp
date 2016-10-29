#include <Windows.h>
#include "LuaEngine.h"
#include <cstdint>
#include <base/hook/fp_call.h>		 	  		
#include <base/filesystem.h>
#include <base/exception/exception.h>
#include <base/hook/inline.h>
#include <base/lua/luabind.h>
#include <base/path/service.h>
#include <base/path/self.h>
#include <base/win/file_version.h>
#include <base/file/stream.h>
#include <base/util/format.h>
#include <base/win/version.h>

int luaopen_log(lua_State* L);

uintptr_t RealLuaPcall = (uintptr_t)::GetProcAddress(::GetModuleHandleW(L"luacore.dll"), "lua_pcallk");
int FakeLuaPcall(lua_State *L, int nargs, int nresults, int errfunc)
{
	EXCEPTION_POINTERS* xp = nullptr;
	int results = 0;
	__try
	{
		results = base::c_call<int>(RealLuaPcall, L, nargs, nresults, errfunc);
	}
	__except (xp = GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER)
	{
		char buf[256];
		sprintf(buf, "SEH exception: '0x%08X'", xp->ExceptionRecord->ExceptionCode);
		lua_pushstring(L, buf);
		return LUA_ERRRUN;
	}

	return results;
}

LuaEngine::LuaEngine()
	: state_(nullptr)
	, logger_()
	, vaild_(false)
{ }

LuaEngine::~LuaEngine()
{
	state_ = nullptr;
	vaild_ = false;
}

bool LuaEngine::InitializeLogger(const fs::path& root_path)
{
	if (!logging::initialize(root_path.c_str(), L"ydwe"))
	{
		printf("initialize error %d\n", GetLastError());
		return false;
	}

	logger_ = logging::get_logger("root");

	LOGGING_INFO(logger_) << "------------------------------------------------------";

	return true;
}

bool LuaEngine::InitializeInfo()
{
	base::win::version_number vn = base::win::get_version_number();

	LOGGING_INFO(logger_) << base::format("YDWE Script engine %s started.", base::win::file_version(base::path::self().c_str())[L"FileVersion"]);
	LOGGING_INFO(logger_) << "Compiled at " __TIME__ ", " __DATE__;
	LOGGING_INFO(logger_) << base::format("Windows version: %d.%d.%d", vn.major, vn.minor, vn.build);

	return true;
}

bool LuaEngine::InitializeLua()
{
#ifndef _DEBUG
	base::hook::inline_install(&RealLuaPcall, (uintptr_t)FakeLuaPcall);
#endif
	state_ = luaL_newstate();
	if (!state_)
	{
		LOGGING_FATAL(logger_) << "Could not initialize script engine. Program may not work correctly!";
		return false;
	}

	luaL_openlibs(state_);
	luabind::open(state_);
	luaL_requiref(state_, "log", luaopen_log, 1);
	lua_pop(state_, 1);
	LOGGING_DEBUG(logger_) << "Initialize script engine successfully.";

	return true;
}

bool LuaEngine::Initialize(const fs::path& root_path)
{
	if (vaild_)
		return true;

	if (!InitializeLogger(root_path))
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

		LOGGING_INFO(logger_) << "Script engine startup complete.";
		vaild_ = true;
		return true;
	}
	catch (std::exception const& e)
	{
		LOGGING_ERROR(logger_) << "exception: " << e.what();
		return false;
	}
	catch (...)
	{
		LOGGING_ERROR(logger_) << "unknown exception.";
		return false;
	}
}

bool LuaEngine::Uninitialize()
{
	if (vaild_)
	{
		//lua_close(state_);
		state_ = nullptr;
		LOGGING_INFO(logger_) << "Script engine has been shut down.";
		vaild_ = false;
	}

	return true;
}

bool LuaEngine::LoadFile(fs::path const& file_path)
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
		std::vector<char> buffer = base::file::read_stream(file_path).read<std::vector<char>>();
		if (luaL_loadbuffer(state_, buffer.data(), buffer.size(), name.c_str()))
		{
			throw luabind::error(state_);
		}

		if (lua_pcall(state_, 0, LUA_MULTRET, 0))
		{
			throw luabind::error(state_);
		}

		return true;
	}
	catch (luabind::error const& e)
	{
		LOGGING_ERROR(logger_) << "exception: " << lua_tostring(e.state(), -1);
		lua_pop(e.state(), 1);
	}
	catch (std::exception const& e)
	{
		LOGGING_ERROR(logger_) << "exception: " << e.what();
	}
	catch (...)
	{
		LOGGING_ERROR(logger_) << "unknown exception.";
	}

	return false;
}

bool LuaEngine::SetPath(fs::path const& path)
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

bool LuaEngine::SetCPath(fs::path const& cpath)
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
