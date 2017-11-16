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

std::wstring str_replace(const wchar_t* str, wchar_t src, wchar_t dst)
{
	std::wstring s(str);
	wchar_t ary_dst[] = { dst, 0 };
	for (size_t pos = 0; (pos = s.find(src, pos)) != std::wstring::npos; pos++)
	{
		s.replace(pos, 1, ary_dst);
	}
	return s;
}

lua_State* LuaEngineCreate(const wchar_t* name)
{
	fs::path ydwe = base::path::self().remove_filename().remove_filename();

	std::unique_ptr<logging::manager> mgr = std::make_unique<logging::manager>((ydwe / L"logs").c_str(), name);

	logging::logger* lg = mgr->get_logger("root");
	LOGGING_INFO(lg) << "------------------------------------------------------";

	base::win::version_number vn = base::win::get_version_number();
	LOGGING_INFO(lg) << base::format("LuaEngine %s started.", base::win::file_version(base::path::self().c_str())[L"FileVersion"]);
	LOGGING_INFO(lg) << "Compiled at " __TIME__ ", " __DATE__;
	LOGGING_INFO(lg) << base::format("Windows version: %d.%d.%d", vn.major, vn.minor, vn.build);

#ifndef _DEBUG
	base::hook::inline_install(&RealLuaPcall, (uintptr_t)FakeLuaPcall);
#endif
	lua_State* L = luaL_newstate();
	if (!L)
	{
		LOGGING_FATAL(lg) << "Could not initialize LuaEngine. Program may not work correctly!";
		return nullptr;
	}

	try
	{
		logging::set_manager(L, mgr.release());

		luaL_openlibs(L);
		luaL_requiref(L, "log", luaopen_log, 1);
		lua_pop(L, 1);
		LOGGING_DEBUG(lg) << "Initialize LuaEngine successfully.";

		fs::path cp = ydwe / L"bin" / L"modules" / L"?.dll";
		lua_getglobal(L, "package");
		lua_pushstring(L, cp.string().c_str());
		lua_setfield(L, -2, "cpath");
		lua_pop(L, 1);

		fs::path ydwedev = ydwe.parent_path().remove_filename().remove_filename();
		if (fs::exists(ydwedev / "build.root")) {
			ydwe = ydwedev / L"Component";
		}

		fs::path p1 = ydwe / L"share" / L"script" / "common" / L"?.lua";
		fs::path p2 = ydwe / L"share" / L"script" / str_replace(name, L'.', L'/') / L"?.lua";
		lua_getglobal(L, "package");
		lua_pushstring(L, (p1.string() + ";" + p2.string()).c_str());
		lua_setfield(L, -2, "path");
		lua_pop(L, 1);
		return L;
	}
	catch (std::exception const& e)
	{
		LOGGING_ERROR(lg) << "exception: " << e.what();
		LuaEngineDestory(L);
		return nullptr;
	}
	catch (...)
	{
		LOGGING_ERROR(lg) << "unknown exception.";
		LuaEngineDestory(L);
		return nullptr;
	}
}

void LuaEngineDestory(lua_State* L)
{
	logging::manager* mgr = logging::get_manager(L);
	lua_close(L);
	L = nullptr;
	if (mgr) {
		LOGGING_INFO(mgr->get_logger("root")) << "LuaEngine has been shut down.";
		delete mgr;
	}
}

bool LuaEngineStart(lua_State* L)
{
	lua_pushcfunction(L, errorfunc);
	lua_getglobal(L, "require");
	lua_pushstring(L, "main");
	if (LUA_OK != lua_pcall(L, 1, 0, -3))
	{
		logging::manager* mgr = logging::get_manager(L);
		LOGGING_ERROR(mgr->get_logger("root")) << "exception: " << lua_tostring(L, -1);
		lua_pop(L, 1);
		return false;
	}
	return true;
}
