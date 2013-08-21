#pragma once

#include <boost/filesystem.hpp>
#include <lua.hpp>
#pragma warning(push)
#pragma warning(disable: 4231)
#pragma warning(disable: 4251)
#include <log4cxx/logger.h>
#pragma warning(pop)

class LuaEngineImpl
{
public:
	LuaEngineImpl();
	~LuaEngineImpl();
	bool Initialize(boost::filesystem::path const& log_config);
	bool InitializeLogger(boost::filesystem::path const& log_config);
	bool InitializeInfo();
	bool InitializeLua();
	bool Uninitialize();
	bool SetPath(boost::filesystem::path const& path);
	bool SetCPath(boost::filesystem::path const& cpath);
	bool LoadFile(boost::filesystem::path const& file_path);

private:
	lua_State*              state_;
	log4cxx::LoggerPtr      logger_;
	bool                    vaild_;
};
