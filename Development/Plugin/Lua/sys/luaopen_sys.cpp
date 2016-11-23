
#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)			 	  		
#include <base/filesystem.h>
#include <Windows.h>
#include <base/win/process.h>
#include <base/win/file_version.h>
#include <fcntl.h>
#include <io.h>	  
#include "NtQuerySystemProcessInformation.h"

namespace NLuaAPI { namespace NSys {

	namespace detail
	{
		std::optional<fs::path> cast_path_opt(const luabind::object &object)
		{
			std::optional<fs::path> path_opt;

			switch (luabind::type(object))
			{
			case LUA_TSTRING:
			{
				boost::optional<std::wstring> opt = luabind::object_cast_nothrow<std::wstring>(object);
				if (opt)
				{
					path_opt = fs::path(opt.get());
				}
			}
			break;
			case LUA_TUSERDATA:
			{
				boost::optional<fs::path> opt = luabind::object_cast_nothrow<fs::path>(object);
				if (opt)
				{
					path_opt = opt.get();
				}
			}
			break;
			default:
				break;
			}

			return std::move(path_opt);
		}

		std::optional<std::wstring> cast_wstring_opt(const luabind::object &object)
		{
			std::optional<std::wstring> str_opt;

			switch (luabind::type(object))
			{
			case LUA_TSTRING:
			{
				boost::optional<std::wstring> opt = luabind::object_cast_nothrow<std::wstring>(object);
				if (opt)
				{
					str_opt = opt.get();
				}
			}
				break;
			case LUA_TUSERDATA:
			{
				boost::optional<fs::path> opt = luabind::object_cast_nothrow<fs::path>(object);
				if (opt)
				{
					str_opt = opt->wstring();
				}
			}
			break;
			default:
				break;
			}

			return std::move(str_opt);
		}

		fs::path cast_path(const luabind::object &object)
		{
			std::optional<fs::path> path_opt = cast_path_opt(object);
			return path_opt ? std::move(path_opt.get()) : std::move(fs::path());
		}

		std::wstring cast_wstring(const luabind::object &object)
		{
			std::optional<std::wstring> str_opt = cast_wstring_opt(object);
			return str_opt ? std::move(str_opt.get()) : std::move(std::wstring());
		}

		HANDLE cast_file_handle(const luabind::object &object)
		{
			boost::optional<HANDLE> handle_opt;

			switch (luabind::type(object))
			{
			case LUA_TUSERDATA:
				{
					boost::optional<void*> ptr_opt = luabind::object_cast_nothrow<void*>(object);
					if (ptr_opt)
					{
						void* ptr = ptr_opt.get();
						if (ptr)
						{
							handle_opt = (HANDLE)_get_osfhandle(fileno(*(FILE **)ptr));
						}
					}
				}
				break;
			default:
				break;
			}

			return handle_opt ? handle_opt.get() : NULL;
		}
	}

	static void LuaProcessCreate(lua_State *pState, base::win::process& p, const luabind::object &application_object, const luabind::object &commandline_object, const luabind::object &currentdirectory_object)
	{
		lua_pushboolean(pState, p.create(
			detail::cast_path_opt(application_object),
			detail::cast_wstring(commandline_object),
			detail::cast_path_opt(currentdirectory_object)
		));
	}

	static void LuaProcessRedirect(lua_State *pState, base::win::process& p, const luabind::object &stdinput_object, const luabind::object &stdoutput_object, const luabind::object &stderror_object)
	{
		HANDLE stdinput  = detail::cast_file_handle(stdinput_object);
		HANDLE stdoutput = detail::cast_file_handle(stdoutput_object);
		HANDLE stderror  = detail::cast_file_handle(stderror_object);
		bool result = p.redirect(stdinput, stdoutput, stderror);
		lua_pushboolean(pState, result);
	}
}}

#define tolstream(L, idx)	((luaL_Stream*)luaL_checkudata(L, idx, LUA_FILEHANDLE))

static luaL_Stream* LuaNewFile(lua_State* L)
{

	luaL_Stream *p = (luaL_Stream*)lua_newuserdata(L, sizeof(luaL_Stream));
	p->closef = NULL;
	luaL_setmetatable(L, LUA_FILEHANDLE);
	return p;
}

static int LuaFileClose(lua_State* L)
{
	luaL_Stream* p = tolstream(L, 1);
	int ok = fclose(p->f);
	int en = errno;  /* calls to Lua API may change this value */
	if (ok)
	{
		lua_pushboolean(L, 1);
		return 1;
	}
	else
	{
		lua_pushnil(L);
		lua_pushfstring(L, "%s", strerror(en));
		lua_pushinteger(L, en);
		return 3;
	}
}

static void LuaOpenFileForHandle(lua_State* L, HANDLE h, int dmode, const char* mode)
{
	luaL_Stream* pf = LuaNewFile(L);
	pf->f = _fdopen(_open_osfhandle((long)h, dmode), mode);
	pf->closef = &LuaFileClose;
}

static int LuaOpenPipe(lua_State* L)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	HANDLE read_pipe, write_pipe;
	if (!::CreatePipe(&read_pipe, &write_pipe, &sa, 0))
	{
		lua_pushnil(L);
		lua_pushnil(L);
		return 2;
	}
	::SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0);
	::SetHandleInformation(write_pipe, HANDLE_FLAG_INHERIT, 0);
	LuaOpenFileForHandle(L, read_pipe, _O_RDONLY | _O_TEXT, "rt");
	LuaOpenFileForHandle(L, write_pipe, _O_WRONLY | _O_TEXT, "wt");
	return 2;
}

static int LuaGetVersionNumberString(lua_State* L)
{
	const fs::path& module = *(fs::path*)luaL_checkudata(L, 1, "filesystem");
	base::win::simple_file_version fv(module.c_str(), L"FileVersion", L',');
	if (fv.size < 2)
	{
		fv = base::win::simple_file_version(module.c_str(), L"FileVersion", L'.');
	}

	lua_newtable(L);
	lua_pushstring(L, "major");
	lua_pushinteger(L, fv.major);
	lua_rawset(L, -3);
	lua_pushstring(L, "minor");
	lua_pushinteger(L, fv.minor);
	lua_rawset(L, -3);
	lua_pushstring(L, "revision");
	lua_pushinteger(L, fv.revision);
	lua_rawset(L, -3);
	lua_pushstring(L, "build");
	lua_pushinteger(L, fv.build);
	lua_rawset(L, -3);
	return 1;
}

static int LuaGetProcessList(lua_State* L)
{
	lua_newtable(L);
	bool suc = NtQuerySystemProcessInformation([=](DWORD ProcessId, PWSTR ImageName, USHORT ImageNameLength)
	{
		std::wstring name(ImageName, ImageNameLength / sizeof(wchar_t));
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		luabind::object(L, name).push(L);

		lua_pushvalue(L, -1);
		lua_rawget(L, -3);
		if (lua_isnoneornil(L, -1))
		{
			lua_pop(L, 1);
			lua_newtable(L);
			{
				lua_pushinteger(L, ProcessId);
				lua_rawseti(L, -2, 1);
			}
			lua_rawset(L, -3);
		}
		else
		{
			size_t n = lua_rawlen(L, -1);
			lua_pushinteger(L, ProcessId);
			lua_rawseti(L, -2, n + 1);
			lua_pop(L, 2);
		}
	});
	if (!suc) {
		lua_pop(L, 1);
		lua_pushnil(L);
	}
	return 1;
}

static int LuaKillProcess(lua_State* L)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, false, (int)luaL_checkinteger(L, 1));
	if (hProcess == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}
	if (!TerminateProcess(hProcess, -1))
	{
		CloseHandle(hProcess);
		lua_pushboolean(L, 0);
		return 1;
	}
	if (WAIT_TIMEOUT == WaitForSingleObject(hProcess, 5000))
	{
		CloseHandle(hProcess);
		lua_pushboolean(L, 0);
		return 1;
	}
	CloseHandle(hProcess);
	lua_pushboolean(L, 1);
	return 1;
}

extern "C" __declspec(dllexport) int luaopen_sys(lua_State* L);

int luaopen_sys(lua_State* L)
{
	using namespace luabind;

	// Bind sys
	module(L, "sys")
	[
		class_<base::win::process>("process")
			.def(constructor<>())
			.def("inject",      &base::win::process::inject)
			.def("hide_window", &base::win::process::hide_window)
			.def("redirect",    &NLuaAPI::NSys::LuaProcessRedirect)
			.def("create",      &NLuaAPI::NSys::LuaProcessCreate)
			.def("wait",        (uint32_t (base::win::process::*)())&base::win::process::wait)
			.def("close",       &base::win::process::close)
			.def("id",          &base::win::process::id)
	];

	lua_getglobal(L, "sys");
	luaL_Reg l2[] = {
		{ "open_pipe", LuaOpenPipe },
		{ "get_module_version_info", LuaGetVersionNumberString },
		{ NULL, NULL },
	};
	luaL_setfuncs(L, l2, 0);

	luaL_Reg l3[] = {
		{ "list", LuaGetProcessList },
		{ "kill", LuaKillProcess },
		{ NULL, NULL },
	};
	luaL_newlib(L, l3);
	lua_setglobal(L, "process");
	return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}
