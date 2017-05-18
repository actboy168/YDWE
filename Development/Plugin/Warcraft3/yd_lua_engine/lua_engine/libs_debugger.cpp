#include <Windows.h>
#include <lua.hpp>
#include <base/warcraft3/command_line.h>

namespace base { namespace warcraft3 { namespace lua_engine { namespace debugger {
	int open(lua_State* L)
	{
		HMODULE m = GetModuleHandleW(L"debugger.dll");
		if (!m) {
			base::warcraft3::command_line cmd;
			if (cmd.has(L"debugger")) {
				m = LoadLibraryW(cmd[L"debugger"].c_str());
			}
			else {
				m = LoadLibraryW(L"debugger.dll");
			}
			if (!m) {
				return 0;
			}
		}
		lua_CFunction openf = (lua_CFunction)GetProcAddress(m, "luaopen_debugger");
		if (!openf) {
			return 0;
		}
		return openf(L);
	}
}}}}
