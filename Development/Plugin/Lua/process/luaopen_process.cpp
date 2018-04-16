
#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)			 	  		
#include <base/filesystem.h>
#include <Windows.h>
#include <base/win/process.h>
#include <base/win/file_version.h>
#include <base/util/unicode.h>
#include <base/util/console.h>
#include <fcntl.h>
#include <io.h>	  

namespace process {
	namespace pipe {

		static luaL_Stream* to(lua_State* L, int idx)
		{
			return ((luaL_Stream*)luaL_checkudata(L, idx, LUA_FILEHANDLE));
		}

		static int close(lua_State* L)
		{
			luaL_Stream* p = to(L, 1);
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

		static std::pair<HANDLE, HANDLE> open(lua_State* L)
		{
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = FALSE;
			sa.lpSecurityDescriptor = NULL;
			HANDLE read_pipe = NULL, write_pipe = NULL;
			if (!::CreatePipe(&read_pipe, &write_pipe, &sa, 0))
			{
				return std::make_pair((HANDLE)NULL, (HANDLE)NULL);
			}
			return std::make_pair(read_pipe, write_pipe);
		}

		static int push(lua_State* L, HANDLE h, int dmode, const char* mode)
		{
			luaL_Stream* pf = (luaL_Stream*)lua_newuserdata(L, sizeof(luaL_Stream));
			pf->closef = NULL;
			luaL_setmetatable(L, LUA_FILEHANDLE);
			pf->f = _fdopen(_open_osfhandle((long)h, dmode), mode);
			pf->closef = &close;
			return 1;
		}

		static int push(lua_State* L, HANDLE h, char type)
		{
			if (!h) {
				return 0;
			}
			if (type == 'r') {
				return push(L, h, _O_RDONLY |_O_TEXT, "rt");
			}
			return push(L, h, _O_WRONLY | _O_TEXT, "wt");
		}

		static int peek(lua_State* L, int idx)
		{
			luaL_Stream* p = to(L, idx);
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
	}

	base::win::process& to(lua_State* L, int idx)
	{
		return *(base::win::process*)luaL_checkudata(L, idx, "process");
	}

	HANDLE cast_file_handle(lua_State* L, int idx)
	{
		if (LUA_TUSERDATA == lua_type(L, idx))
		{
			luaL_Stream* p = pipe::to(L, idx);
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
		bool ok = self.inject_x86(*(fs::path*)luaL_checkudata(L, 2, "filesystem"));
		lua_pushboolean(L, ok);
		return 1;
	}

	int inject_x86(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		bool ok = self.inject_x86(*(fs::path*)luaL_checkudata(L, 2, "filesystem"));
		lua_pushboolean(L, ok);
		return 1;
	}

	int inject_x64(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		bool ok = self.inject_x64(*(fs::path*)luaL_checkudata(L, 2, "filesystem"));
		lua_pushboolean(L, ok);
		return 1;
	}

	int set_console(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		const char* console = luaL_checkstring(L, 2);
		if (strcmp(console, "new") == 0) {
			lua_pushboolean(L, self.set_console(base::win::process::CONSOLE_NEW));
		}
		else if(strcmp(console, "disable") == 0) {
			lua_pushboolean(L, self.set_console(base::win::process::CONSOLE_DISABLE));
		}
		else if (strcmp(console, "inherit") == 0) {
			lua_pushboolean(L, self.set_console(base::win::process::CONSOLE_INHERIT));
		}
		else {
			lua_pushboolean(L, 0);
		}
		return 1;
	}

	int hide_window(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		bool ok = self.hide_window();
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

	int kill(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		bool ok = self.kill(5000);
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

	int std_input(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		auto[rd, wr] = pipe::open(L);
		if (!rd || !wr) {
			return 0;
		}
		if (!self.redirect(rd, NULL, NULL)) {
			::CloseHandle(rd);
			::CloseHandle(wr);
			return 0;
		}
		return pipe::push(L, wr, 'w');
	}

	int std_output(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		auto [rd, wr] = pipe::open(L);
		if (!rd || !wr) {
			return 0;
		}
		if (!self.redirect(NULL, wr, NULL)) {
			::CloseHandle(rd);
			::CloseHandle(wr);
			return 0;
		}
		return pipe::push(L, rd, 'r');
	}

	int std_error(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		auto[rd, wr] = pipe::open(L);
		if (!rd || !wr) {
			return 0;
		}
		if (!self.redirect(NULL, NULL, wr)) {
			::CloseHandle(rd);
			::CloseHandle(wr);
			return 0;
		}
		return pipe::push(L, rd, 'r');
	}

	int peek(lua_State* L)
	{
		return pipe::peek(L, 2);
	}

	int set_env(lua_State* L)
	{
		base::win::process& self = to(L, 1);
		if (lua_type(L, 3) == LUA_TNIL) {
			self.del_env(luaL_checkwstring(L, 2));
		}
		else {
			self.set_env(luaL_checkwstring(L, 2), luaL_checkwstring(L, 3));
		}
		return 0;
	}
}

extern "C" __declspec(dllexport)
int luaopen_process(lua_State* L)
{
	static luaL_Reg mt[] = {
		{ "inject", process::inject },
		{ "inject_x86", process::inject_x86 },
		{ "inject_x64", process::inject_x64 },
		{ "hide_window", process::hide_window },
		{ "set_console", process::set_console },
		{ "std_input", process::std_input },
		{ "std_output", process::std_output },
		{ "std_error", process::std_error },
		{ "peek", process::peek },
		{ "create", process::create },
		{ "wait", process::wait },
		{ "close", process::close },
		{ "kill", process::kill },
		{ "id", process::id },
		{ "is_running", process::is_running },
		{ "set_env", process::set_env },
		{ "__gc", process::destructor },
		{ NULL, NULL }
	};
	luaL_newmetatable(L, "process");
	luaL_setfuncs(L, mt, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, process::constructor);
	return 1;
}

int set_filemode(lua_State* L) {
	luaL_Stream* p = process::pipe::to(L, 1);
	const char* mode = luaL_checkstring(L, 2);
	if (p && p->f) {
		if (mode[0] == 'b') {
			_setmode(_fileno(p->f), _O_BINARY);
		}
		else {
			_setmode(_fileno(p->f), _O_TEXT);
		}
	}
	return 0;
}

extern "C" __declspec(dllexport)
int luaopen_process_ext(lua_State* L)
{
	static luaL_Reg mt[] = {
		{ "set_filemode", set_filemode },
		{ NULL, NULL },
	};
	luaL_newlib(L, mt);
	return 1;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}
