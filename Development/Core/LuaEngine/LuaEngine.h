#pragma once

#include <base/filesystem.h>
#include <lua.hpp>
#include "logging.h"

lua_State* LuaEngineCreate(const wchar_t* name);
void       LuaEngineDestory(lua_State* L);
