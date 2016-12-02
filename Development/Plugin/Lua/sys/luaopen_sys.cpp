
#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)			 	  		
#include <base/filesystem.h>
#include <Windows.h>
#include <base/win/process.h>
#include <base/win/file_version.h>
#include <base/util/unicode.h>
#include <fcntl.h>
#include <io.h>	  
#include "NtQuerySystemProcessInformation.h"

static void lua_pushwstring(lua_State* L, const std::wstring& str)
{
	std::string ustr = base::w2u(str, base::conv_method::replace | '?');
	lua_pushlstring(L, ustr.data(), ustr.size());
}

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

static int LuaPeekPipe(lua_State* L)
{
	luaL_Stream* p = tolstream(L, 1);
	if (p) {
		DWORD rlen = 0;
		if (PeekNamedPipe((HANDLE)_get_osfhandle(_fileno(p->f)), 0, 0, 0, &rlen, 0)) {
			lua_pushinteger(L, rlen);
			return 1;
		}
	}
	lua_pushinteger(L, 0);
	return 1;
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

		lua_pushwstring(L, name);
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

namespace process {

	base::win::process& to(lua_State* L, int idx)
	{
		return *(base::win::process*)luaL_checkudata(L, idx, "process");
	}

	HANDLE cast_file_handle(lua_State* L, int idx)
	{
		if (LUA_TUSERDATA == lua_type(L, idx))
		{
			luaL_Stream* p = tolstream(L, idx);
			if (p)
			{
				return (HANDLE)_get_osfhandle(fileno(p->f));
			}
		}
		return NULL;
	}

	std::wstring luaL_checkwstring(lua_State* L, int idx)
	{
		size_t len = 0;
		const char* str = luaL_checklstring(L, idx, &len);
		return base::u2w(std::string_view(str, len), base::conv_method::replace | '?');
	}

	std::optional<fs::path> cast_path_opt(lua_State* L, int idx)
	{
		switch (lua_type(L, idx))
		{
		case LUA_TSTRING:
			return fs::path(luaL_checkwstring(L, idx));
		case LUA_TUSERDATA:
			return *(fs::path*)luaL_checkudata(L, idx, "filesystem");
		default:
			break;
		}
		return std::optional<fs::path>();
	}

	std::wstring cast_wstring(lua_State* L, int idx)
	{
		switch (lua_type(L, idx))
		{
		case LUA_TSTRING:
			return luaL_checkwstring(L, idx);
		case LUA_TUSERDATA: {
			fs::path& p = *(fs::path*)luaL_checkudata(L, idx, "filesystem");
			return p.wstring();
		}
		default:
			break;
		}
		return std::wstring();
	}

	int constructor(lua_State* L)
	{
		void* storage = lua_newuserdata(L, sizeof(base::win::process));
		luaL_getmetatable(L, "process");
		lua_setmetatable(L, -2);
		new (storage)base::win::process();
		return 1;
	}

	static int destructor(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		self.~process();
		return 0;
	}

	int inject(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		bool ok = self.inject(*(fs::path*)luaL_checkudata(L, 2, "filesystem"));
		lua_pushboolean(L, ok);
		return 1;
	}

	int hide_window(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		bool ok = self.hide_window();
		lua_pushboolean(L, ok);
		return 1;
	}

	int redirect(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		bool ok = self.redirect(cast_file_handle(L, 2), cast_file_handle(L, 3), cast_file_handle(L, 4));
		lua_pushboolean(L, ok);
		return 1;
	}

	int create(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		bool ok = self.create(cast_path_opt(L, 2), cast_wstring(L, 3), cast_path_opt(L, 4));
		lua_pushboolean(L, ok);
		return 1;
	}

	int wait(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		lua_pushinteger(L, (lua_Integer)self.wait());
		return 1;
	}

	int close(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		bool ok = self.close();
		lua_pushboolean(L, ok);
		return 1;
	}

	int id(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		lua_pushinteger(L, (lua_Integer)self.id());
		return 1;
	}

	int is_running(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		lua_pushboolean(L, self.is_running());
		return 1;
	}
}

extern "C" __declspec(dllexport) int luaopen_sys(lua_State* L);

int luaopen_sys(lua_State* L)
{
	static luaL_Reg mt[] = {
		{ "inject", process::inject },
		{ "hide_window", process::hide_window },
		{ "redirect", process::redirect },
		{ "create", process::create },
		{ "wait", process::wait },
		{ "close", process::close },
		{ "id", process::id },
		{ "is_running", process::is_running },
		{ "__gc", process::destructor },
		{ NULL, NULL }
	};
	luaL_newmetatable(L, "process");
	luaL_setfuncs(L, mt, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_Reg l2[] = {
		{ "process", process::constructor },
		{ "open_pipe", LuaOpenPipe },
		{ "peek_pipe", LuaPeekPipe },
		{ "get_module_version_info", LuaGetVersionNumberString },
		{ NULL, NULL },
	};
	luaL_newlib(L, l2);
	lua_setglobal(L, "sys");

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
