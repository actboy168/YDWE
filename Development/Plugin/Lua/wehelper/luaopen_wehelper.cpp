#include <lua.hpp>

extern "C"
#if defined(_WIN32)
__declspec(dllexport)
#endif
int luaopen_wehelper(lua_State* L) {
	return 0;
}
