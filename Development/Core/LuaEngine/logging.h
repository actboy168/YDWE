#pragma once

#include "logging_backend.h"
#include "logging_logger.h"	 
#include <base/util/unicode.h>

#define LOGGING_STREAM(lg, lvl) for (bool f = true; f; f = false) logging::make_record_pump((lg), (lvl)).stream()
#define LOGGING_TRACE(lg)   LOGGING_STREAM(*(lg), logging::trace)
#define LOGGING_DEBUG(lg)   LOGGING_STREAM(*(lg), logging::debug)
#define LOGGING_INFO(lg)    LOGGING_STREAM(*(lg), logging::info)
#define LOGGING_WARNING(lg) LOGGING_STREAM(*(lg), logging::warning)
#define LOGGING_ERROR(lg)   LOGGING_STREAM(*(lg), logging::error)
#define LOGGING_FATAL(lg)   LOGGING_STREAM(*(lg), logging::fatal)

#if defined(LUAENGINE_EXPORTS)
#	define LUAENGINE_API __declspec(dllexport)
#else
#	define LUAENGINE_API __declspec(dllimport)
#endif

inline std::ostream& operator <<(std::ostream& os, const std::wstring& val)
{
	os << base::w2u(val);
	return os;
}

namespace logging
{
	typedef logger_t<sync_frontend<backend>>  logger;

	LUAENGINE_API bool    initialize(const fs::path& root, const std::wstring& name);
	LUAENGINE_API logger* get_logger(const char* name);
}
