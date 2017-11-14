#include "LuaEngine.h"
#include <Windows.h>
#include <stdint.h>
#include <base/hook/fp_call.h>	  		
#include <base/filesystem.h>
#include <base/hook/inline.h>
#include <base/path/self.h>
#include <base/win/file_version.h>
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

static int errorfunc(lua_State* L)
{
	luaL_traceback(L, L, lua_tostring(L, 1), 0);
	return 1;
}

LuaEngine::LuaEngine()
	: L(nullptr)
{ }

LuaEngine::~LuaEngine()
{
	L = nullptr;
}

bool LuaEngine::Initialize(const std::wstring& name)
{
	fs::path ydwe = base::path::self().remove_filename().remove_filename();

	logging::manager* mgr = new logging::manager((ydwe / L"logs").c_str(), name);

	logging::logger* lg = mgr->get_logger("root");
	LOGGING_INFO(lg) << "------------------------------------------------------";

	try
	{
		base::win::version_number vn = base::win::get_version_number();
		LOGGING_INFO(lg) << base::format("LuaEngine %s started.", base::win::file_version(base::path::self().c_str())[L"FileVersion"]);
		LOGGING_INFO(lg) << "Compiled at " __TIME__ ", " __DATE__;
		LOGGING_INFO(lg) << base::format("Windows version: %d.%d.%d", vn.major, vn.minor, vn.build);

#ifndef _DEBUG
		base::hook::inline_install(&RealLuaPcall, (uintptr_t)FakeLuaPcall);
#endif
		L = luaL_newstate();

		if (!L)
		{
			LOGGING_FATAL(lg) << "Could not initialize LuaEngine. Program may not work correctly!";
			return false;
		}

		logging::set_manager(L, mgr);

		luaL_openlibs(L);
		luaL_requiref(L, "log", luaopen_log, 1);
		lua_pop(L, 1);
		LOGGING_DEBUG(lg) << "Initialize LuaEngine successfully.";

		SetCPath(ydwe / L"bin" / L"modules" / L"?.dll");
		fs::path ydwedev = ydwe.parent_path().remove_filename().remove_filename();
		if (fs::exists(ydwedev / "build.root")) {
			ydwe = ydwedev / L"Component";
		}
		SetPath(ydwe / L"share" / L"script" / "common" / L"?.lua", ydwe / L"share" / L"script" / name / L"?.lua");

		lua_pushcfunction(L, errorfunc);
		lua_getglobal(L, "require");
		lua_pushstring(L, "main");
		if (LUA_OK != lua_pcall(L, 1, 0, -3))
		{
			LOGGING_ERROR(lg) << "exception: " << lua_tostring(L, -1);
			lua_pop(L, 1);
			return false;
		}
		return true;
	}
	catch (std::exception const& e)
	{
		LOGGING_ERROR(lg) << "exception: " << e.what();
		Uninitialize();
		return false;
	}
	catch (...)
	{
		LOGGING_ERROR(lg) << "unknown exception.";
		Uninitialize();
		return false;
	}
}

bool LuaEngine::Uninitialize()
{
	if (L)
	{
		logging::manager* mgr = logging::get_manager(L);
		lua_close(L);
		L = nullptr;
		if (mgr) {
			LOGGING_INFO(mgr->get_logger("root")) << "LuaEngine has been shut down.";
			delete mgr;
		}
	}
	return true;
}

bool LuaEngine::SetPath(fs::path const& p1, fs::path const& p2)
{
	if (!L) return false;
	lua_getglobal(L, "package");
	lua_pushstring(L, (p1.string() + ";" + p2.string()).c_str());
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
