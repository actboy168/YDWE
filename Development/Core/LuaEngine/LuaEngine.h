#pragma once

#include <base/filesystem.h>
#include <lua.hpp>

lua_State* LuaEngineCreate(const wchar_t* name);
void       LuaEngineDestory(lua_State* L);
bool       LuaEngineStart(lua_State* L);
