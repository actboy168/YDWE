#include "LuaEngine.h"
#include "LuaEngineImpl.h"

LuaEngine::LuaEngine()
	: pimpl_(new LuaEngineImpl)
{ }

LuaEngine::~LuaEngine()
{ }

bool LuaEngine::Initialize(boost::filesystem::path const& log_config)
{
	return pimpl_->Initialize(log_config);
}

bool LuaEngine::Uninitialize()
{
	return pimpl_->Uninitialize();
}

bool LuaEngine::LoadFile(boost::filesystem::path const& file_path)
{
	return pimpl_->LoadFile(file_path);
}

bool LuaEngine::SetPath(boost::filesystem::path const& path)
{
	return pimpl_->SetPath(path);
}

bool LuaEngine::SetCPath(boost::filesystem::path const& cpath)
{
	return pimpl_->SetCPath(cpath);
}
