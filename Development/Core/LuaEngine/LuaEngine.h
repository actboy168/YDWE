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
	bool InitializeLogger(const fs::path& root_path);
	bool InitializeInfo();
	bool InitializeLua();
	bool Uninitialize();
	bool SetPath(fs::path const& path);
	bool SetCPath(fs::path const& cpath);
	bool LoadFile(fs::path const& file_path);

private:
	lua_State*              state_;
	logging::logger*        logger_;
	bool                    vaild_;
};
