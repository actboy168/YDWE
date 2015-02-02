#pragma once

#include <boost/filesystem.hpp>

class LuaEngineImpl;

class LuaEngine
{
public:
	LuaEngine();
	~LuaEngine();
	bool Initialize(boost::filesystem::path const& log_config);
	bool Uninitialize();
	bool SetPath(boost::filesystem::path const& path);
	bool SetCPath(boost::filesystem::path const& cpath);
	bool LoadFile(boost::filesystem::path const& file_path);

private:
#pragma warning(push)
#pragma warning(disable:4251)	
	std::unique_ptr<LuaEngineImpl> pimpl_;
#pragma warning(pop)
};
