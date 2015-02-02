#pragma once

#include <boost/filesystem.hpp>
#include <lua.hpp>
#include <../Plugin/Lua/log/logging.h>

class LuaEngineImpl
{
public:
	LuaEngineImpl();
	~LuaEngineImpl();
	bool Initialize(const boost::filesystem::path& root_path);
	bool InitializeLogger(const boost::filesystem::path& root_path);
	bool InitializeInfo();
	bool InitializeLua();
	bool Uninitialize();
	bool SetPath(boost::filesystem::path const& path);
	bool SetCPath(boost::filesystem::path const& cpath);
	bool LoadFile(boost::filesystem::path const& file_path);

private:
	lua_State*              state_;
	logging::logger         logger_;
	bool                    vaild_;
};
