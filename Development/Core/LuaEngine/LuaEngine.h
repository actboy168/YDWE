#pragma once

#include <base/filesystem.h>
#include <lua.hpp>
#include "logging.h"

LUAENGINE_API lua_State* LuaEngineCreate(const wchar_t* name);
LUAENGINE_API void       LuaEngineDestory(lua_State* L);
