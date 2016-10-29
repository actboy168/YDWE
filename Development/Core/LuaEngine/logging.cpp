#include "logging.h"
#include "logging_backend.h" 
#include <map>

namespace logging
{
	std::map<std::string, logger*> loggers;
	fs::path                        loggers_root;
	std::wstring                                   loggers_name;

	bool initialize(const fs::path& root, const std::wstring& name)
	{
		loggers_root = root;
		loggers_name = name;
		return true;
	}

	logger* get_logger(const char* name)
	{
		auto it = loggers.find(name);
		if (it != loggers.end())
		{
			return it->second;
		}

		loggers[name] = new logger(name, backend(loggers_root, loggers_name));
		return loggers[name];
	}
}
