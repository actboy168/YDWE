#include "callback.h"
#include "common.h"
#include <warcraft3/jass.h>
#include <warcraft3/war3_searcher.h>
#include <base/hook/fp_call.h>
#include <bee/registry.h>
#include <bee/utility/unicode_win.h>
#include <bee/utility/path_helper.h>
#include <set>
#include <Windows.h>
#include <io.h>
#include "storm.h"
#include "lua_memfile.h"

namespace fs = std::filesystem;

namespace warcraft3::lua_engine {

	int IFileCreate(const char* filename, unsigned char** data, size_t* size)
	{
		if (storm_s::instance().load_file(filename, (const void**)data, size))
		{
			return 0;
		}
		return ENOENT;
	}

	int IFileClose(unsigned char* data, size_t /*size*/)
	{
		storm_s::instance().unload_file(data);
		return 0;
	}

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
		int searcher_lua(lua_State *L);
		int searcher_dll(lua_State *L);
		const char *searchpath(lua_State *L, const char *name, const char *path, const char *sep, const char *dirsep, bool is_local = true);
	}

	int package_searchers(lua_State* L)
	{
		lua_newtable(L);
		lua_pushcclosure(L, package::searcher_preload, 0);
		lua_rawseti(L, -2, 1);
		lua_pushvalue(L, -3);
		lua_pushcclosure(L, package::searcher_lua, 1);
		lua_rawseti(L, -2, 2);
		lua_pushvalue(L, -3);
		lua_pushcclosure(L, package::searcher_dll, 1);
		lua_rawseti(L, -2, 3);
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

	static bool allow_local_files()
	{
		try {
			return !!bee::registry::key_w(L"HKEY_CURRENT_USER\\Software\\Blizzard Entertainment\\Warcraft III")[L"Allow Local Files"].get_uint32_t();
		}
		catch (bee::registry::registry_exception const&) {}
		return false;
	}

	static int create_readfile(lua_State *L, const char* mode) {
		if (allow_local_files())
		{
			const char *filename = luaL_checkstring(L, 1);
			try {
				fs::path filepath = bee::u2w(filename);
				if (!filepath.is_absolute()) {
					filepath = bee::path_helper::exe_path().value().parent_path() / filepath;
				}
				if (fs::exists(filepath))
				{
					luaL_Stream *p = (luaL_Stream *)lua_newuserdata(L, sizeof(luaL_Stream));
					luaL_setmetatable(L, LUA_FILEHANDLE);
					p->f = fopen(filepath.string().c_str(), mode);
					p->closef = &io_fclose;
					return 1;
				}
			}
			catch (...) {
			}
		}
		return memfile::open(L, IFileCreate, IFileClose);
	}

	static bool can_write(const char* filename, luaL_Stream* p, const char* mode)
	{
		static std::set<std::wstring> s_blacklist = { L"mix", L"asi", L"m3d", L"flt", L"flt", L"exe", L"dll" };
		try {
			fs::path rootpath = bee::path_helper::exe_path().value().parent_path();
			fs::path filepath = rootpath / bee::u2w(filename);
			filepath = fs::absolute(filepath);

			std::wstring ext = filepath.extension().wstring().substr(1, 4);
			std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
			if (s_blacklist.find(ext) != s_blacklist.end())
			{
				return false;
			}
			for (fs::path path = filepath.parent_path(); !path.empty(); path = path.parent_path())
			{
				if (bee::path_helper::equal(path, rootpath))
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

	static int create_writefile(lua_State *L, const char* mode) {
		const char *filename = luaL_checkstring(L, 1);
		luaL_Stream *p = (luaL_Stream *)lua_newuserdata(L, sizeof(luaL_Stream));
		luaL_setmetatable(L, LUA_FILEHANDLE);
		p->closef = 0;
		p->f = 0;
		if (!can_write(filename, p, mode)) {
			errno = EACCES;
			return luaL_fileresult(L, 0, filename);
		}
		return (p == NULL || p->f == NULL) ? luaL_fileresult(L, 0, filename) : 1;
	}

	static int createfile(lua_State *L, const char* mode) {
		if (mode[0] == 'r') {
			return create_readfile(L, mode);
		}
		return create_writefile(L, mode);
	}

	static int l_checkmode(const char *mode) {
		return *mode != '\0'
			&& strchr("rwa", *(mode++)) != NULL
			&& (*mode != '+' || (++mode, 1))
			&& (strspn(mode, "b") == strlen(mode));
	}

	int io_open(lua_State* L)
	{
		const char *mode = luaL_optstring(L, 2, "r");
		luaL_argcheck(L, l_checkmode(mode), 2, "invalid mode");
		return createfile(L, mode);
	}

	int fix_baselib(lua_State* L)
	{
		memfile::initialize(L);

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
			lua_pushstring(L, "stdin");     lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "stdout");    lua_pushnil(L); lua_rawset(L, -3);
			lua_pushstring(L, "stderr");    lua_pushnil(L); lua_rawset(L, -3);
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

	uintptr_t get_random_seed()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = s.search_string("SetRandomSeed");
		ptr = *(uintptr_t*)(ptr + 0x05);
		ptr = next_opcode(ptr, 0x8B, 6);
		ptr = *(uintptr_t*)(ptr + 2);
		return *(uintptr_t*)(*(uintptr_t*)(ptr)+4);
	}

	lua_State* newstate()
	{
		putenv(("LUA_SEED=" + std::to_string(get_random_seed())).c_str());
		lua_State* L = luaL_newstate();
		putenv("LUA_SEED=");
		return L;
	}
}
