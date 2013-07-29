#pragma once

#include <boost/filesystem.hpp>


#if defined(LUAENGINE_EXPORTS)
#	define LUAENGINE_API __declspec(dllexport)
#else
#	define LUAENGINE_API __declspec(dllimport)
#endif

class LuaEngineImpl;

class LUAENGINE_API LuaEngine
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
