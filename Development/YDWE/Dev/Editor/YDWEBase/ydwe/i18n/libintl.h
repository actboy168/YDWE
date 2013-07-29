#pragma once

#include <ydwe/config.h>
#include <boost/filesystem.hpp>

namespace ydwe { namespace i18n {
	YDWE_BASE_API bool        bindtextdomain(const char* domain, boost::filesystem::path const& mofile_path);
	YDWE_BASE_API const char* textdomain(const char* domain);
	YDWE_BASE_API const char* gettext(const char* orig_str);
	YDWE_BASE_API const char* dgettext(const char* domain, const char* orig_str);
}}
