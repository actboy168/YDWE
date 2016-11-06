
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

	static void LuaGetVersionNumberString(lua_State *pState, const fs::path &module)
	{
		lua_newtable(pState);
		luabind::object version_table(luabind::from_stack(pState, -1));
		base::win::simple_file_version fv(module.c_str(), L"FileVersion", L',');
		if (fv.size >= 2)
		{
			version_table["major"] = fv.major;
			version_table["minor"] = fv.minor;
			version_table["revision"] = fv.revision;
			version_table["build"] = fv.build;
		}
		else
		{
			base::win::simple_file_version fv(module.c_str(), L"FileVersion", L'.');
			version_table["major"] = fv.major;
			version_table["minor"] = fv.minor;
			version_table["revision"] = fv.revision;
			version_table["build"] = fv.build;
		}
		version_table.push(pState);
	}

#define tolstream(L, idx)	((luaL_Stream*)luaL_checkudata(L, idx, LUA_FILEHANDLE))

	static luaL_Stream* LuaNewFile(lua_State* pState) 
	{

		luaL_Stream *p = (luaL_Stream*)lua_newuserdata(pState, sizeof(luaL_Stream));
		p->closef = NULL;
		luaL_setmetatable(pState, LUA_FILEHANDLE);
		return p;
	}

	static int LuaFileClose(lua_State* pState)
	{
		luaL_Stream* p = tolstream(pState, 1);
		int ok = fclose(p->f);
		int en = errno;  /* calls to Lua API may change this value */
		if (ok)
		{
			lua_pushboolean(pState, 1);
			return 1;
		}
		else 
		{
			lua_pushnil(pState);
			lua_pushfstring(pState, "%s", strerror(en));
			lua_pushinteger(pState, en);
			return 3;
		}
	}

	static void LuaOpenFileForHandle(lua_State* pState, HANDLE h, int dmode, const char* mode)
	{
		luaL_Stream* pf = LuaNewFile(pState);
		pf->f      = _fdopen(_open_osfhandle((long)h, dmode), mode);
		pf->closef = &LuaFileClose;
	}
	
	static void LuaOpenPipe(lua_State *pState)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;

		HANDLE read_pipe, write_pipe;
		if (!::CreatePipe(&read_pipe, &write_pipe, &sa, 0))
		{
			lua_pushnil(pState);
			lua_pushnil(pState);
			return ;
		}
		::SetHandleInformation(read_pipe,  HANDLE_FLAG_INHERIT, 0);
		::SetHandleInformation(write_pipe, HANDLE_FLAG_INHERIT, 0);
		LuaOpenFileForHandle(pState, read_pipe,  _O_RDONLY | _O_TEXT, "rt");
		LuaOpenFileForHandle(pState, write_pipe, _O_WRONLY | _O_TEXT, "wt");
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

	static int LuaKillProcess(lua_State* L, int processid)
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, false, processid);
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
}}

extern "C" __declspec(dllexport) int luaopen_sys(lua_State* L);

int luaopen_sys(lua_State *pState)
{
	using namespace luabind;

	// Bind sys
	module(pState, "sys")
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
		,

		def("open_pipe", &NLuaAPI::NSys::LuaOpenPipe),
		def("get_module_version_info", &NLuaAPI::NSys::LuaGetVersionNumberString)
	];

	module(pState, "process")
	[
		def("list", &NLuaAPI::NSys::LuaGetProcessList),
		def("kill", &NLuaAPI::NSys::LuaKillProcess)
	];

	return 0;
}

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}
