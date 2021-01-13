#include <Windows.h>
#include <lua.hpp>
#include <base/filesystem.h>
#include <regex>
#include <map>
#include <fstream>

namespace warcraft3::lua_engine::debugger {
    static fs::path getLatestDebugger() {
        fs::path extensionPath = fs::path(getenv("USERPROFILE")) / ".vscode" / "extensions";
        const std::regex matchVersion(R"(^actboy168\.lua\-debug\-([0-9]+)\.([0-9]+)\.([0-9]+)$)");
        std::cmatch matcher;
        std::map<long, fs::path, std::greater<>> luaDebugs;
        for (auto& p : fs::directory_iterator(extensionPath)) {
            std::string filename = p.path().filename().string();
            if (std::regex_match(filename.c_str(), matcher, matchVersion)) {
                long a = std::stol(matcher.str(1));
                long b = std::stol(matcher.str(2));
                long c = std::stol(matcher.str(3));
                luaDebugs[a * 10000 + b * 100 + c] = p.path();
            }
        }
        if (luaDebugs.empty()) {
            printf("Cant find `actboy168.lua-debug`.\n");
            return fs::path();
        }
        return luaDebugs.begin()->second;
    }

    static bool readall(fs::path const& path, std::string& buf){
        std::ifstream f(path.c_str(), std::ios::binary);
        if (!f) {
            return false;
        }
        buf = std::string((std::istreambuf_iterator<char>(f)), (std::istreambuf_iterator<char>()));
        f.close();
        return true;
    }

    static void pusherror(lua_State* L) {
        int error = GetLastError();
        char buffer[128];
        if (FormatMessageA(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, error, 0, buffer, sizeof(buffer) / sizeof(char), NULL))
            lua_pushstring(L, buffer);
        else
            lua_pushfstring(L, "system error %d\n", error);
    }

    static int loadlib(lua_State* L) {
        const char* path = luaL_checkstring(L, 1);
        const char* init = luaL_checkstring(L, 2);
        HMODULE lib = LoadLibraryA(path);
        if (lib == NULL) {
            luaL_pushfail(L);
            pusherror(L);
            return 2;
        }
        lua_CFunction f = (lua_CFunction)GetProcAddress((HMODULE)lib, init);
        if (f == NULL) {
            luaL_pushfail(L);
            pusherror(L);
            return 2;
        }
        lua_pushcfunction(L, f);
        return 1;
    }

#define DoString(L, s, n) \
	(luaL_loadbuffer(L, s, sizeof(s) - 1, "module '" #s "'") || (lua_insert(L, -(n)-1), lua_pcall(L, n, LUA_MULTRET, 0)))

	int open(lua_State* L, int port) {
        fs::path path = getLatestDebugger();
        if (path.empty()) {
            printf("Cant find `actboy168.lua-debug`.\n");
            return 0;
        }
        std::string debugger;
        if (!readall(path / "script" / "debugger.lua", debugger)) {
            printf("Cant load `actboy168.lua-debug`.\n");
            return 0;
        }
        lua_pushstring(L, path.string().c_str());
        lua_pushstring(L, debugger.c_str());
		lua_pushinteger(L, port);
        lua_pushcfunction(L, loadlib);
		if (DoString(L, R"=(
local PATH, DEBUG, PORT, LOADLIB = ...
package.loadlib = LOADLIB
local init_debug = assert(load(DEBUG, "=(BOOTSTRAP)"))
local dbg = init_debug(PATH)
dbg:start("127.0.0.1:"..PORT)
return dbg
		)=", 4)) {
            printf("%s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
			return 0;
		}
		return 1;
	}
}
