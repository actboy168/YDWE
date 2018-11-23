#pragma once

#include <base/filesystem.h>
#include <lua.hpp>

#if defined(LUAENGINE_EXPORTS)
#	define LUAENGINE_API __declspec(dllexport)
#else
#	define LUAENGINE_API __declspec(dllimport)
#endif


LUAENGINE_API lua_State* LuaEngineCreate(const wchar_t* name);
LUAENGINE_API void       LuaEngineDestory(lua_State* L);
LUAENGINE_API bool       LuaEngineStart(lua_State* L);
