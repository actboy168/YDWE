#include <lua.hpp>
#include <string.h>	  
#include <base/util/unicode.h>	 
#include <base/path/service.h>
#include <base/path/helper.h>
#include <base/file/stream.h>
#include "storm.h"

namespace base { namespace warcraft3 { namespace lua_engine { namespace package {
#if !defined (LUA_PATH_SEP)
#define LUA_PATH_SEP		";"
#endif
#if !defined (LUA_PATH_MARK)
#define LUA_PATH_MARK		"?"
#endif
#if !defined (LUA_EXEC_DIR)
#define LUA_EXEC_DIR		"!"
#endif
#if !defined (LUA_IGMARK)
#define LUA_IGMARK		"-"
#endif

#if !defined(LUA_CSUBSEP)
#define LUA_CSUBSEP		LUA_DIRSEP
#endif

#if !defined(LUA_LSUBSEP)
#define LUA_LSUBSEP		LUA_DIRSEP
#endif

	static int readable(const char *filename, bool is_local) {
		if (is_local) {
			try {
				std::wstring wfilename = base::u2w(filename);
				std::ifstream fs(wfilename.c_str(), std::ios::binary | std::ios::in);
				if (fs.is_open())
				{
					return 1;
				}
			}
			catch (...) {}
			return 0;
		}
		else {
			return storm_s::instance().has_file(filename) ? 1 : 0;
		}
	}

	static const char *pushnexttemplate(lua_State *L, const char *path) {
		const char *l;
		while (*path == *LUA_PATH_SEP) path++;  /* skip separators */
		if (*path == '\0') return NULL;  /* no more templates */
		l = strchr(path, *LUA_PATH_SEP);  /* find next separator */
		if (l == NULL) l = path + strlen(path);
		lua_pushlstring(L, path, l - path);  /* template */
		return l;
	}

	static const char *searchpath(lua_State *L, const char *name, const char *path, const char *sep, const char *dirsep, bool is_local) {
		luaL_Buffer msg;  /* to build error message */
		luaL_buffinit(L, &msg);
		if (*sep != '\0')  /* non-empty separator? */
			name = luaL_gsub(L, name, sep, dirsep);  /* replace it by 'dirsep' */
		while ((path = pushnexttemplate(L, path)) != NULL) {
			const char *filename = luaL_gsub(L, lua_tostring(L, -1),
				LUA_PATH_MARK, name);
			lua_remove(L, -2);  /* remove path template */
			if (readable(filename, is_local))  /* does file exist and is readable? */
				return filename;  /* return that file name */
			lua_pushfstring(L, "\n\tno file " LUA_QS, filename);
			lua_remove(L, -2);  /* remove file name */
			luaL_addvalue(&msg);  /* concatenate error msg. entry */
		}
		luaL_pushresult(&msg);  /* create error message */
		return NULL;  /* not found */
	}

	static const char *findfile(lua_State *L, const char *name, const char *pname, const char *dirsep, bool is_local) {
		const char *path;
		lua_getfield(L, lua_upvalueindex(1), pname);
		path = lua_tostring(L, -1);
		if (path == NULL)
			luaL_error(L, LUA_QL("package.%s") " must be a string", pname);
		return searchpath(L, name, path, ".", dirsep, is_local);
	}

	static int checkload(lua_State *L, int stat, const char *filename) {
		if (stat) {  /* module loaded successfully? */
			lua_pushstring(L, filename);  /* will be 2nd argument to module */
			return 2;  /* return open function and file name */
		}
		else
			return luaL_error(L, "error loading module " LUA_QS " from file " LUA_QS ":\n\t%s", lua_tostring(L, 1), filename, lua_tostring(L, -1));
	}

	int searcher_storm(lua_State *L) {
		const char *filename;
		size_t      size = 0;
		const char *name = luaL_checklstring(L, 1, &size);
		filename = findfile(L, name, "path", LUA_LSUBSEP, false);
		if (filename == NULL) return 1;  /* module not found in this path */
		const char* buffer = nullptr;
		storm_dll& s = storm_s::instance();
		size = 0;
		s.load_file(filename, (const void**)&buffer, &size);
		lua_pushfstring(L, "@%s", filename);
		int idx = lua_gettop(L);
		int stat = (luaL_loadbuffer(L, buffer, size, lua_tostring(L, -1)) == LUA_OK);
		lua_remove(L, idx);
		s.unload_file(buffer);
		return checkload(L, stat, filename);
	}

	int searcher_file(lua_State *L) {
		const char *filename;
		size_t      size = 0;
		const char *name = luaL_checklstring(L, 1, &size);
		filename = findfile(L, name, "path", LUA_LSUBSEP, true);
		if (filename == NULL) return 1;  /* module not found in this path */
		int stat = 0;
		try {
			std::wstring wfilename = base::u2w(filename);
			std::string buffer = base::file::read_stream(wfilename).read<std::string>();
			lua_pushfstring(L, "@%s", filename);
			int idx = lua_gettop(L);
			stat = (luaL_loadbuffer(L, buffer.c_str(), buffer.size(), lua_tostring(L, -1)) == LUA_OK);
			lua_remove(L, idx);
		}
		catch (...) {}
		return checkload(L, stat, filename);
	}
}}}}
