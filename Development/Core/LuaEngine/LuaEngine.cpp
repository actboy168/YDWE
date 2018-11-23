#include "LuaEngine.h"
#include <Windows.h>
#include <stdint.h>
#include <base/hook/fp_call.h>	  		
#include <base/filesystem.h>
#include <base/hook/inline.h>
#include <base/path/self.h>
#include <base/path/ydwe.h>
#include <base/win/file_version.h>
#include <base/util/format.h>
#include <base/win/version.h>
#include "../../Plugin/Lua/log/logging.h"

void lua_pushwstring(lua_State* L, const std::wstring& str)
{
	std::string ustr = bee::w2u(str);
	lua_pushlstring(L, ustr.data(), ustr.size());
}

uintptr_t RealLuaPcall = (uintptr_t)::GetProcAddress(::GetModuleHandleW(L"lua54.dll"), "lua_pcallk");
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
	fs::path ydwe = base::path::ydwe(false);
	fs::path ydwedev = base::path::ydwe(true);
#ifndef _DEBUG
	base::hook::install(&RealLuaPcall, (uintptr_t)FakeLuaPcall);
#endif
	lua_State* L = luaL_newstate();
	if (!L) {
		return nullptr;
	}

	logging::logger* lg = logging::create(L, (ydwe / L"logs").wstring(), name);
	try
	{
		LOGGING_INFO(lg) << "------------------------------------------------------";

		base::win::version_number vn = base::win::get_version_number();
		LOGGING_INFO(lg) << base::format("LuaEngine %s started.", base::win::file_version(base::path::self().c_str())[L"FileVersion"]);
		LOGGING_INFO(lg) << "Compiled at " __TIME__ ", " __DATE__;
		LOGGING_INFO(lg) << base::format("Windows version: %d.%d.%d", vn.major, vn.minor, vn.build);

		luaL_openlibs(L);
		LOGGING_DEBUG(lg) << "Initialize LuaEngine successfully.";

		fs::path cp = ydwe / L"bin" / L"?.dll";
		lua_getglobal(L, "package");
		lua_pushwstring(L, cp.wstring());
		lua_setfield(L, -2, "cpath");
		lua_pop(L, 1);

		fs::path p1 = ydwedev / L"script" / "common" / L"?.lua";
		fs::path p2 = ydwedev / L"script" / str_replace(name, L'.', L'/') / L"?.lua";
		lua_getglobal(L, "package");
		lua_pushwstring(L, p1.wstring() + L";" + p2.wstring());
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
	logging::logger* lg = logging::get(L);
	if (lg) {
		LOGGING_INFO(lg) << "LuaEngine has been shut down.";
	}
	lua_close(L);
}

bool LuaEngineStart(lua_State* L)
{
	lua_pushcfunction(L, errorfunc);
	lua_getglobal(L, "require");
	lua_pushstring(L, "main");
	if (LUA_OK != lua_pcall(L, 1, 0, -3))
	{
		LOGGING_ERROR(logging::get(L)) << "exception: " << lua_tostring(L, -1);
		lua_pop(L, 2);
		return false;
	}
	lua_pop(L, 1);
	return true;
}
