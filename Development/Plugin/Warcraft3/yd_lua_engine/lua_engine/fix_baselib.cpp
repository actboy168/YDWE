#include "callback.h"
#include "lua_helper.h"
#include "common.h"
#include <base/warcraft3/jass.h>

#include <base/hook/fp_call.h>
#include <base/win/registry/key.h>	
#include <base/util/unicode.h>	
#include <base/path/service.h>
#include <base/path/helper.h>
#include <base/util/list_of.h>
#include <set>
#include <Windows.h>
#include <io.h>
#include "storm.h"

namespace base { namespace warcraft3 { namespace lua_engine {

	int math_random(lua_State* L)
	{
		lua_Integer low = 0, up = 1;
		double r;
		if (is_gaming())
		{
			jass::jreal_t a = jass::to_real(0.f);
			jass::jreal_t b = jass::to_real(1.f);
			r = (double)jass::from_real(jass::call("GetRandomReal", &a, &b));
		}
		else
		{
			r = (double)rand() * (1.0 / ((double)RAND_MAX + 1.0));
		}

		switch (lua_gettop(L)) 
		{
		case 0:
			lua_pushnumber(L, (lua_Number)r);
			break;
		case 1:
			low = 1;
			up = luaL_checkinteger(L, 1);
			break;
		case 2:
			low = luaL_checkinteger(L, 1);
			up = luaL_checkinteger(L, 2);
			break;
		default:
			return luaL_error(L, "wrong number of arguments");
		}
		luaL_argcheck(L, low <= up, 1, "interval is empty");
		luaL_argcheck(L, low >= 0 || up <= LUA_MAXINTEGER + low, 1, "interval too large");
		r *= (double)(up - low) + 1.0;
		lua_pushinteger(L, (lua_Integer)r + low);
		return 1;
	}

	int math_randomseed(lua_State* L)
	{
		if (is_gaming())
		{
			jass::call("SetRandomSeed", (unsigned int)(lua_Integer)luaL_checknumber(L, 1));
		}
		else
		{
			srand((unsigned int)(lua_Integer)luaL_checknumber(L, 1));
			(void)rand();
		}

		return 0;
	}

	namespace package {
		int searcher_preload(lua_State *L);
		int searcher_file(lua_State *L);
		int searcher_dll(lua_State *L);
		int searcher_storm(lua_State *L);
		const char *searchpath(lua_State *L, const char *name, const char *path, const char *sep, const char *dirsep);
	}

	int package_searchers(lua_State* L)
	{
		lua_newtable(L);
		lua_pushcclosure(L, package::searcher_preload, 0);
		lua_rawseti(L, -2, 1);
		lua_pushvalue(L, -3);
		lua_pushcclosure(L, package::searcher_file, 1);
		lua_rawseti(L, -2, 2);
		lua_pushvalue(L, -3);
		lua_pushcclosure(L, package::searcher_storm, 1);
		lua_rawseti(L, -2, 3);
		lua_pushvalue(L, -3);
		lua_pushcclosure(L, package::searcher_dll, 1);
		lua_rawseti(L, -2, 4);
		return 1;
	}

	int package_searchpath(lua_State* L)
	{
		const char* f = package::searchpath(L, luaL_checkstring(L, 1), luaL_checkstring(L, 2), luaL_optstring(L, 3, "."), luaL_optstring(L, 4, LUA_DIRSEP));
		if (f != NULL) return 1;
		lua_pushnil(L);
		lua_insert(L, -2);
		return 2;
	}

	static int io_fclose(lua_State *L) {
		luaL_Stream *p = ((luaL_Stream *)luaL_checkudata(L, 1, LUA_FILEHANDLE));
		int res = fclose(p->f);
		return luaL_fileresult(L, (res == 0), NULL);
	}

	static int io_fclose_and_delete(lua_State *L) {
		luaL_Stream *p = ((luaL_Stream *)luaL_checkudata(L, 1, LUA_FILEHANDLE));
		wchar_t path[MAX_PATH] = { 0 };
		uintptr_t func = (uintptr_t)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetFinalPathNameByHandleW");
		if (func)
		{
			HANDLE hfile = (HANDLE)_get_osfhandle(fileno(p->f));
			if (std_call<DWORD>(func, hfile, path, MAX_PATH, VOLUME_NAME_DOS) >= MAX_PATH)
			{
				path[0] = L'\0';
			}
		}
		int res = fclose(p->f);
		if (path[0])
		{
			DeleteFileW(path);
		}
		return luaL_fileresult(L, (res == 0), NULL);
	}

	static bool allow_local_files()
	{
		try {
			return !!registry::key_w(HKEY_CURRENT_USER, L"", L"Software\\Blizzard Entertainment\\Warcraft III")[L"Allow Local Files"].get<uint32_t>();
		}
		catch (registry::registry_exception const&) {}
		return false;
	}

	static bool can_read(const char* filename, luaL_Stream* p, const char* mode)
	{
		if (allow_local_files())
		{
			try {
				fs::path filepath = u2w(filename);
				if (!filepath.is_absolute()) {
					filepath = path::get(path::DIR_EXE).remove_filename() / filepath;
				}
				if (fs::exists(filepath))
				{
					p->f = fopen(filepath.string().c_str(), mode);
					p->closef = &io_fclose;
					return true;
				}
			}
			catch (...) {
			}
		}

		try {
			const void* buf = nullptr;
			size_t      len = 0;
			storm_dll&s = storm_s::instance();
			if (!s.load_file(filename, &buf, &len))
			{
				return false;
			}

			fs::path file = path::get(path::DIR_TEMP) / filename;
			fs::create_directories(file.parent_path());
			FILE* tmp = fopen(file.string().c_str(), "wb");
			if (!tmp) {
				return false;
			}
			if (1 != fwrite(buf, len, 1, tmp)) {
				fclose(tmp);
				return false;
			}
			fclose(tmp);
			s.unload_file(buf);

			p->f = fopen(file.string().c_str(), mode);
			p->closef = &io_fclose_and_delete;
			return true;
		}
		catch (...) {
		}

		return false;
	}

	static bool can_write(const char* filename, luaL_Stream* p, const char* mode)
	{
		static std::set<std::wstring> s_blacklist = list_of(L"mix")(L"asi")(L"m3d")(L"flt")(L"flt")(L"exe")(L"dll");
		try {
			fs::path rootpath = path::get(path::DIR_EXE).remove_filename();
			fs::path filepath = rootpath / u2w(filename);
			filepath = path::normalize(filepath);

			std::wstring ext = filepath.extension().wstring().substr(1, 4);
			std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
			if (s_blacklist.find(ext) != s_blacklist.end())
			{
				return false;
			}
			for (fs::path path = filepath.parent_path(); !path.empty(); path = path.parent_path())
			{
				if (path::equal(path, rootpath))
				{
					fs::create_directories(filepath.parent_path());
					p->f = fopen(filepath.string().c_str(), mode);
					p->closef = &io_fclose;
					return true;
				}
			}
			return false;
		}
		catch (...) {}
		return false;
	}

	static luaL_Stream* createfile(lua_State *L, const char* filename, const char* mode) {
		luaL_Stream *p = (luaL_Stream *)lua_newuserdata(L, sizeof(luaL_Stream));
		luaL_setmetatable(L, LUA_FILEHANDLE);
		p->closef = 0;
		p->f = 0;
		if (mode[0] == 'r') {
			if (!can_read(filename, p, mode)) {
				errno = ENOENT;
				return 0;
			}
		}
		else {
			if (!can_write(filename, p, mode)) {
				errno = EACCES;
				return 0;
			}
		}
		return p;
	}

	static int l_checkmode(const char *mode) {
		return *mode != '\0'
			&& strchr("rwa", *(mode++)) != NULL
			&& (*mode != '+' || (++mode, 1))
			&& (strspn(mode, "b") == strlen(mode));
	}

	int io_open(lua_State* L)
	{
		const char *filename = luaL_checkstring(L, 1);
		const char *mode = luaL_optstring(L, 2, "r");
		luaL_argcheck(L, l_checkmode(mode), 2, "invalid mode");
		luaL_Stream *p = createfile(L, filename, mode);
		return (p == NULL || p->f == NULL) ? luaL_fileresult(L, 0, filename) : 1;
	}

	int fix_baselib(lua_State* L)
	{
		if (lua_getglobal(L, "math") == LUA_TTABLE)
		{
			lua_pushstring(L, "random");     lua_pushcclosure(L, math_random, 0);     lua_rawset(L, -3);
			lua_pushstring(L, "randomseed"); lua_pushcclosure(L, math_randomseed, 0); lua_rawset(L, -3);
		}
		lua_pop(L, 1);

		if (lua_getglobal(L, "io") == LUA_TTABLE)
		{
			lua_pushstring(L, "lines");     lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "popen");     lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "tmpfile");   lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "input");     lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "output");    lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "open");      lua_pushcclosure(L, io_open, 0); lua_rawset(L, -3);
		}
		lua_pop(L, 1);

		if (lua_getglobal(L, "os") == LUA_TTABLE)
		{
			lua_pushstring(L, "execute");   lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "exit");      lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "getenv");    lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "remove");    lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "rename");    lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "setlocale"); lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "tmpname");   lua_pushnil(L); lua_rawset(L, -3);
		}
		lua_pop(L, 1);

		if (lua_getglobal(L, "package") == LUA_TTABLE)
		{
			lua_pushstring(L, "loadlib");   lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "searchpath"); lua_pushcclosure(L, package_searchpath, 0); lua_rawset(L, -3);
			lua_pushstring(L, "searchers"); package_searchers(L);  lua_rawset(L, -3);
			lua_pushstring(L, "path");      lua_pushstring(L, "?.lua"); lua_rawset(L, -3);
			lua_pushstring(L, "cpath");     lua_pushstring(L, "?.dll"); lua_rawset(L, -3);
		}
		lua_pop(L, 1);

		lua_pushnil(L);
		lua_setglobal(L, "dofile");
		lua_pushnil(L);
		lua_setglobal(L, "loadfile");
		return 0;
	}
}}}
