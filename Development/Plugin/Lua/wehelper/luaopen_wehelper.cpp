#include <lua.hpp>
#include "YDWELogger.h"
#include "YDWEHook.h"

namespace NYDWE
{
	logging::logger* lg;
}

extern "C"
#if defined(_WIN32)
__declspec(dllexport)
#endif
int luaopen_wehelper(lua_State* L) {
	NYDWE::lg = logging::get_logger(L);
	NYDWE::InstallHooks();
	LOGGING_INFO(NYDWE::lg) << "YDWE startup complete.";
	return 0;
}
