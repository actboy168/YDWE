#pragma once

#pragma warning(push)
#pragma warning(disable: 4819)
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#pragma warning(pop)

#define LOGGING_TRACE(lg)   BOOST_LOG_SEV(lg, boost::log::trivial::trace)
#define LOGGING_DEBUG(lg)   BOOST_LOG_SEV(lg, boost::log::trivial::debug)
#define LOGGING_INFO(lg)    BOOST_LOG_SEV(lg, boost::log::trivial::info)
#define LOGGING_WARNING(lg) BOOST_LOG_SEV(lg, boost::log::trivial::warning)
#define LOGGING_ERROR(lg)   BOOST_LOG_SEV(lg, boost::log::trivial::error)
#define LOGGING_FATAL(lg)   BOOST_LOG_SEV(lg, boost::log::trivial::fatal)

#if defined(LUAENGINE_EXPORTS)
#	define LUAENGINE_API __declspec(dllexport)
#else
#	define LUAENGINE_API __declspec(dllimport)
#endif

namespace logging
{
	typedef boost::log::sources::severity_logger<boost::log::trivial::severity_level>  logger;
	typedef boost::log::sources::wseverity_logger<boost::log::trivial::severity_level> wlogger;

	LUAENGINE_API bool    initiate(const boost::filesystem::path& root, const std::wstring& name);
	LUAENGINE_API logger  get_logger(const char* name);
	LUAENGINE_API wlogger get_wlogger(const char* name);
}
