#pragma once

#include "logging_backend.h"
#include "logging_logger.h"	 
#include <bee/utility/unicode.h>
#include <map>

struct lua_State;

#define LOGGING_STREAM(lg, lvl) for (bool f = true; f; f = false) logging::make_record_pump((lg), (lvl)).stream()
#define LOGGING_TRACE(lg)   LOGGING_STREAM(*(lg), logging::trace)
#define LOGGING_DEBUG(lg)   LOGGING_STREAM(*(lg), logging::debug)
#define LOGGING_INFO(lg)    LOGGING_STREAM(*(lg), logging::info)
#define LOGGING_WARNING(lg) LOGGING_STREAM(*(lg), logging::warn)
#define LOGGING_ERROR(lg)   LOGGING_STREAM(*(lg), logging::error)
#define LOGGING_FATAL(lg)   LOGGING_STREAM(*(lg), logging::fatal)

#if defined(LUAENGINE_EXPORTS)
#	define LUAENGINE_API __declspec(dllexport)
#else
#	define LUAENGINE_API __declspec(dllimport)
#endif

inline std::ostream& operator <<(std::ostream& os, const std::wstring& val)
{
	os << bee::w2u(val);
	return os;
}

namespace logging
{
	struct logger : public logger_t<sync_frontend<backend>> {
		logger(const fs::path& root, const std::wstring& name)
			: logger_t<sync_frontend<backend>>(backend(root, name))
		{ }
	};

	LUAENGINE_API void     set_logger(lua_State* L, logger* lg);
	LUAENGINE_API logger*  get_logger(lua_State* L);
}
