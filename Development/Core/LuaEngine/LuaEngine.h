#pragma once

#include <base/filesystem.h>
#include <lua.hpp>
#include "logging.h"

class LuaEngine
{
public:
	LuaEngine();
	~LuaEngine();
	bool Initialize(const std::wstring& name);
	bool Uninitialize();

private:
	bool SetPath(fs::path const& p1, fs::path const& p2);
	bool SetCPath(fs::path const& cpath);

private:
	lua_State*              L;
};
