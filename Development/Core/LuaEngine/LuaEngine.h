#pragma once

#include <base/filesystem.h>
#include <lua.hpp>
#include "logging.h"

class LuaEngine
{
public:
	LuaEngine();
	~LuaEngine();
	bool Initialize(const fs::path& root_path);
	bool Uninitialize();
	bool SetPath(fs::path const& path);
	bool SetCPath(fs::path const& cpath);
	bool LoadFile(fs::path const& file_path);

private:
	lua_State*              L;
	logging::logger*        logger_;
};
