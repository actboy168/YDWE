#include <Windows.h>
#include "LuaEngine.h"
#include <cstdint>
#include <base/hook/fp_call.h>		 	  		
#include <base/filesystem.h>
#include <base/exception/exception.h>
#include <base/hook/inline.h>
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
		sprintf_s(buf, "SEH exception: '0x%08X'", xp->ExceptionRecord->ExceptionCode);
		lua_pushstring(L, buf);
		return LUA_ERRRUN;
	}

	return results;
}

LuaEngine::LuaEngine()
	: L(nullptr)
	, logger_()
{ }

LuaEngine::~LuaEngine()
{
	L = nullptr;
}

bool LuaEngine::Initialize(const fs::path& root, const std::wstring& name)
{
	if (!logging::initialize(root.c_str(), name))
	{
		printf("initialize error %d\n", GetLastError());
		return false;
	}
	logger_ = logging::get_logger("root");
	LOGGING_INFO(logger_) << "------------------------------------------------------";

	try
	{
		base::win::version_number vn = base::win::get_version_number();
		LOGGING_INFO(logger_) << base::format("YDWE Script engine %s started.", base::win::file_version(base::path::self().c_str())[L"FileVersion"]);
		LOGGING_INFO(logger_) << "Compiled at " __TIME__ ", " __DATE__;
		LOGGING_INFO(logger_) << base::format("Windows version: %d.%d.%d", vn.major, vn.minor, vn.build);

#ifndef _DEBUG
		base::hook::inline_install(&RealLuaPcall, (uintptr_t)FakeLuaPcall);
#endif
		L = luaL_newstate();
		if (!L)
		{
			LOGGING_FATAL(logger_) << "Could not initialize script engine. Program may not work correctly!";
			return false;
		}

		luaL_openlibs(L);
		luaL_requiref(L, "log", luaopen_log, 1);
		lua_pop(L, 1);
		LOGGING_DEBUG(logger_) << "Initialize script engine successfully.";
		return true;
	}
	catch (std::exception const& e)
	{
		LOGGING_ERROR(logger_) << "exception: " << e.what();
		Uninitialize();
		return false;
	}
	catch (...)
	{
		LOGGING_ERROR(logger_) << "unknown exception.";
		Uninitialize();
		return false;
	}
}

bool LuaEngine::Uninitialize()
{
	if (L)
	{
		lua_close(L);
		L = nullptr;
		LOGGING_INFO(logger_) << "Script engine has been shut down.";
	}
	return true;
}

struct luaerror {
	luaerror(lua_State* l) : m_l(l) {}
	lua_State* m_l;
	lua_State* state() const { return m_l; }
};

bool LuaEngine::LoadFile(fs::path const& file)
{
	try
	{
		if (!L) return false;
		std::string name = file.string();
		std::vector<char> buffer = base::file::read_stream(file).read<std::vector<char>>();
		if (luaL_loadbuffer(L, buffer.data(), buffer.size(), name.c_str()))
		{
			throw luaerror(L);
		}

		if (lua_pcall(L, 0, LUA_MULTRET, 0))
		{
			throw luaerror(L);
		}

		return true;
	}
	catch (luaerror const& e)
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
	if (!L) return false;
	lua_getglobal(L, "package");
	lua_pushstring(L, path.string().c_str());
	lua_setfield(L, -2, "path");
	lua_pop(L, 1);
	return true;
}

bool LuaEngine::SetCPath(fs::path const& cpath)
{
	if (!L) return false;
	lua_getglobal(L, "package");
	lua_pushstring(L, cpath.string().c_str());
	lua_setfield(L, -2, "cpath");
	lua_pop(L, 1);
	return true;
}
