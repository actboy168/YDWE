#pragma once

#include <base/filesystem.h>
#include <lua.hpp>
#include "logging.h"

class LuaEngine
{
public:
	LuaEngine();
	~LuaEngine();
	bool Initialize(const fs::path& root, const std::wstring& name);
	bool Uninitialize();
	bool SetPath(fs::path const& path);
	bool SetCPath(fs::path const& cpath);
	bool Require(const char* file);

private:
	lua_State*              L;
	logging::logger*        lg;
};
