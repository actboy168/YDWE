#include "logging.h"

#include <boost/locale/generator.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include "logging_backend.h" 

namespace logging
{
	std::map<std::string, logger*> loggers;
	boost::filesystem::path                        loggers_root;
	std::wstring                                   loggers_name;

	bool initialize(const boost::filesystem::path& root, const std::wstring& name)
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

		return loggers[name];
	}
}
